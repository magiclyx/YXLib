//
//  threadpool.c
//  TestSelectServer
//
//  Created by Yuxi Liu on 10/18/12.
//  Copyright (c) 2012 Yuxi Liu. All rights reserved.
//
#include "threadpool.h"

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "../container/thread/yx_core_threadresource_queue.h"
#include "../allocator/thread/yx_core_tsmembucket.h"

#define THREAD_POOL_DEBUG

#ifdef THREAD_POOL_DEBUG
#define REPORT_ERROR(...) fprintf (stderr,"line %d - ",__LINE__); fprintf (stderr, __VA_ARGS__); fprintf (stderr,"\n")
#else
#define REPORT_ERROR(...)
#endif /* THREAD_POOL_DEBUG */

#define THREAD_POOL_DEFAULT_TASK_NUM 2

#define yx_core_threadpool_refHandle2Impl(h) ((threadpoolRef)h)

struct threadpool_task
{
	void (*routine_cb)(void*);

	void *data;
}threadpool_task;

typedef struct threadpool_task* threadpool_task_ref;


typedef struct threadpool
{
    yx_allocator allocator;
    yx_allocator freetask_bucket;
    
	volatile yx_bool stop_flag;

	unsigned short num_of_threads;
    yx_os_thread* thread_array;
//	pthread_t *thr_arr;

    yx_core_resource_queue tasks_queue;
}*threadpoolRef;



static struct threadpool_task* threadpool_task_get_task(struct threadpool *pool)
{
	struct threadpool_task* task;

	if (pool->stop_flag) {
		/* The pool should shut down return NULL. */
		return NULL;
	}
    
    int rt;
    while ((rt = yx_core_resource_queue_get(&(pool->tasks_queue), (yx_value*)(&task), 0.5)) == ETIMEDOUT)
    {
        if (pool->stop_flag) {
            /* The pool should shut down return NULL. */
            return NULL;
        }
    }
    
    if (0 != rt)
        return NULL;
    
    
    return task;
}


static void *worker_thr_routine(void *data)
{
	struct threadpool *pool = (struct threadpool*)data;
	struct threadpool_task *task;

	while (1) {
		task = threadpool_task_get_task(pool);
		if (task == NULL) {
			if (pool->stop_flag) {
				/* Worker thr needs to exit (thread pool was shutdown). */
				break;
			}
			else {
				/* An error has occurred. */
				REPORT_ERROR("Warning an error has occurred when trying to obtain a worker task.");
				REPORT_ERROR("The worker thread has exited.");
                continue;
			}
		}

		/* Execute routine (if any). */
		if (task->routine_cb) {
			task->routine_cb(task->data);
            
            yx_tsmembucket_free(pool->freetask_bucket, task);
		}
	}

	return NULL;
}


static void *stop_worker_thr_routines_cb(void *ptr, yx_bool blocking)
{
	yx_int i;
	struct threadpool *pool = (struct threadpool*)ptr;

    pool->stop_flag = yx_true;
    yx_core_resource_queue_stop(&(pool->tasks_queue));
    
    /*强制 cancel*/
    if (yx_false == blocking) {
        for (i = 0; i < pool->num_of_threads; i++)
            yx_os_thread_cancel(&(pool->thread_array[i]));
    }

	/* Wait until all worker threads are done. */
	for (i = 0; i < pool->num_of_threads; i++) {
		if (yx_os_thread_join(&(pool->thread_array[i]), NULL)) {
			perror("pthread_join: ");
		}
	}
    
    yx_core_resource_queue_recycle(&(pool->tasks_queue));
    
    
    /*free the free task bucket*/
    yx_tsmembucket_destroy(&(pool->freetask_bucket));

	/* Free all allocated memory. */
    yx_allocator_free(pool->allocator, pool->thread_array);
    yx_allocator_free(pool->allocator, pool);

	return NULL;
}

yx_core_threadpool_ref yx_core_threadpool_create(yx_allocator allocator, yx_int num_of_threads)
{
	struct threadpool *pool;
	yx_int i;
    
    if (NULL == allocator)
        allocator = yx_get_allocator();

	/* Create the thread pool struct. */
	if ((pool = (struct threadpool*)yx_allocator_alloc(allocator, sizeof(struct threadpool))) == NULL) {
		perror("malloc: ");
		return NULL;
	}
    
    pool->allocator = allocator;

	pool->stop_flag = yx_false;
    
    
    /* Init the bucket allocator */
    pool->freetask_bucket = yx_tsmembucket_create(allocator, sizeof(struct threadpool_task));

	/* Init the free tasks queue. */
    yx_core_resource_queue_init(allocator, &(pool->tasks_queue));
    
    
    /*warmup the tasks bucket*/
    threadpool_task_ref task_array[THREAD_POOL_DEFAULT_TASK_NUM];
    for (i = 0; i< THREAD_POOL_DEFAULT_TASK_NUM; i++)
        task_array[i] = yx_tsmembucket_alloc(pool->freetask_bucket);
    for (i = 0; i< THREAD_POOL_DEFAULT_TASK_NUM; i++)
        yx_tsmembucket_free(pool->freetask_bucket, task_array[i]);

    
	/* Create the thr_arr. */
	if ((pool->thread_array = (yx_os_thread_ref)yx_allocator_alloc(allocator, sizeof(yx_os_thread) * num_of_threads)) == NULL) {
		perror("malloc: ");
        yx_allocator_free(allocator, pool);
		return NULL;
	}

	/* Start the worker threads. */
	for (pool->num_of_threads = 0; pool->num_of_threads < num_of_threads; (pool->num_of_threads)++) {
        
		if (yx_os_thread_create(&(pool->thread_array[pool->num_of_threads]), worker_thr_routine, pool)) {
			perror("pthread_create:");

			yx_core_theadpool_destroy((yx_core_threadpool_ref*)(&pool), 0);

			return NULL;
		}
	}
    
	return (yx_core_threadpool_ref)pool;
}

yx_int yx_core_thread_addTask(yx_core_threadpool_ref handle, void (*routine)(void*), void *data, yx_int blocking)
{
	struct threadpool_task *task;
    threadpoolRef pool = yx_core_threadpool_refHandle2Impl(handle);
    
	if (pool == NULL) {
		REPORT_ERROR("The threadpool received as argument is NULL.");
		return -1;
	}
    
    
    task = yx_tsmembucket_alloc(pool->freetask_bucket);
    if (NULL == task)
        return -1;
    
	task->data = data;
	task->routine_cb = routine;
    
    
	/* Add the task, to the tasks queue. */
    yx_core_resource_queue_add(&(pool->tasks_queue), (yx_value)task);
    
	return 0;
}

void yx_core_theadpool_destroy(yx_core_threadpool_ref* handleRef, yx_int blocking)
{
    threadpoolRef pool = yx_core_threadpool_refHandle2Impl(*handleRef);
    *handleRef = NULL;
    
	stop_worker_thr_routines_cb(pool, blocking);
    
}

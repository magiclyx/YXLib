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

#define THREAD_POOL_DEBUG

#ifdef THREAD_POOL_DEBUG
#define REPORT_ERROR(...) fprintf (stderr,"line %d - ",__LINE__); fprintf (stderr, __VA_ARGS__); fprintf (stderr,"\n")
#else
#define REPORT_ERROR(...)
#endif /* THREAD_POOL_DEBUG */

#define THREAD_POOL_QUEUE_SIZE 10000


#define yx_core_threadpool_refHandle2Impl(h) ((threadpoolRef)h)

struct threadpool_task
{
	void (*routine_cb)(void*);

	void *data;
};


typedef struct threadpool
{
    yx_allocator allocator;
	struct threadpool_task tasks[THREAD_POOL_QUEUE_SIZE];

	pthread_t *thr_arr;

	unsigned short num_of_threads;
	volatile yx_bool stop_flag;

    yx_core_resource_queue free_tasks_queue;
    yx_core_resource_queue tasks_queue;
}*threadpoolRef;


static void threadpool_task_init(struct threadpool_task *task)
{
	task->data = NULL;
	task->routine_cb = NULL;
}


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

			/* Release the task by returning it to the free_task_queue. */
			threadpool_task_init(task);
            yx_core_resource_queue_add(&(pool->free_tasks_queue), (yx_value)task);
            
		}
	}

	return NULL;
}


static void *stop_worker_thr_routines_cb(void *ptr)
{
	yx_int i;
	struct threadpool *pool = (struct threadpool*)ptr;

    pool->stop_flag = yx_true;
    yx_core_resource_queue_stop(&(pool->free_tasks_queue));
    yx_core_resource_queue_stop(&(pool->tasks_queue));

	/* Wait until all worker threads are done. */
	for (i = 0; i < pool->num_of_threads; i++) {
		if (pthread_join(pool->thr_arr[i],NULL)) {
			perror("pthread_join: ");
		}
	}
    
    yx_core_resource_queue_recycle(&(pool->free_tasks_queue));
    yx_core_resource_queue_recycle(&(pool->tasks_queue));

	/* Free all allocated memory. */
    yx_allocator_free(pool->allocator, pool->thr_arr);
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

	/* Init the jobs queue. */
    yx_core_resource_queue_init(allocator, &(pool->free_tasks_queue));
	/* Init the free tasks queue. */
    yx_core_resource_queue_init(allocator, &(pool->tasks_queue));
    
    
	/* Add all the free tasks to the free tasks queue. */
	for (i = 0; i < THREAD_POOL_QUEUE_SIZE; i++) {
		threadpool_task_init((pool->tasks) + i);
        yx_core_resource_queue_add(&(pool->free_tasks_queue), (yx_value)((pool->tasks) + i));
	}

	/* Create the thr_arr. */
	if ((pool->thr_arr = (pthread_t*)yx_allocator_alloc(allocator, sizeof(pthread_t) * num_of_threads)) == NULL) {
		perror("malloc: ");
        yx_allocator_free(allocator, pool);
		return NULL;
	}

	/* Start the worker threads. */
	for (pool->num_of_threads = 0; pool->num_of_threads < num_of_threads; (pool->num_of_threads)++) {
		if (pthread_create(&(pool->thr_arr[pool->num_of_threads]),NULL,worker_thr_routine,pool)) {
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
    
    
    /* return if queue is empty and !blocking */
    if (!blocking  &&  yx_core_resource_queue_isEmpty(&(pool->free_tasks_queue)))
        return -1;

	/* Check if the free task queue is empty. */
    int rt;
	while ((rt = yx_core_resource_queue_get(&(pool->free_tasks_queue), (yx_value*)(&task), 5)) == ETIMEDOUT)
    {
        if (pool->stop_flag)
            return -1;
	}
    
    if (0 != rt)
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
    
	pthread_t thr;

	if (blocking) {
		stop_worker_thr_routines_cb(pool);
	}
	else {
        /*create a new thread used to recycle the pool thread*/
		if (pthread_create(&thr,NULL,stop_worker_thr_routines_cb,pool)) {
			perror("pthread_create: ");
			REPORT_ERROR("Warning! will not be able to free memory allocation. This will cause a memory leak.");
		}
        
        pthread_detach(thr);
	}
    
    *handleRef = NULL;
}

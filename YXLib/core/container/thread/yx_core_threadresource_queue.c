//
//  yx_core_threadresource_queue.c
//  YXCLib
//
//  Created by Yuxi Liu on 2017/11/30.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#include "yx_core_threadresource_queue.h"


/* create & destroy */
yx_core_resource_queue_ref yx_core_resource_queue_create(yx_allocator allocator)
{
    if (NULL == allocator)
        allocator = yx_get_allocator();

    yx_core_resource_queue_ref queue_ref = (yx_core_resource_queue_ref)yx_allocator_alloc(allocator, sizeof(yx_core_resource_queue));
    
    yx_core_resource_queue_init(allocator, queue_ref);
    
    return queue_ref;
}

void yx_core_resource_queue_init(yx_allocator allocator, yx_core_resource_queue_ref queue_ref)
{
    if (NULL == allocator)
        allocator = yx_get_allocator();
    
    queue_ref->allocator = allocator;
    queue_ref->running = yx_true;
    
    yx_os_pthread_cond_init(&(queue_ref->cond));
    yx_os_pthread_mutex_init(&(queue_ref->mutex));
    yx_os_pthread_rwlock_init(&(queue_ref->rwlock));
    yx_core_queue_init(allocator, &(queue_ref->queue));
}

void yx_core_resource_queue_recycle(yx_core_resource_queue_ref queue_ref)
{
    yx_core_resource_queue_stop(queue_ref);
    
    yx_os_pthread_cond_recycle(&(queue_ref->cond));
    yx_os_pthread_mutex_recycle(&(queue_ref->mutex));
    yx_os_pthread_rwlock_recycle(&(queue_ref->rwlock));
    yx_core_queue_recycle(&(queue_ref->queue));
}

void yx_core_resource_queue_destroy(yx_core_resource_queue_ref* queue_ref_ptr)
{
    yx_core_resource_queue_ref queue_ref = *queue_ref_ptr;
    *queue_ref_ptr = NULL;
    
    
    yx_core_resource_queue_recycle(queue_ref);
    yx_allocator_free(queue_ref->allocator, queue_ref);
}

/* property */
yx_bool yx_core_resource_queue_isEmpty(yx_core_resource_queue_ref queue_ref)
{
    yx_bool isEmpty;
    
    yx_os_pthread_rwlock_rlock(&(queue_ref->rwlock));
    isEmpty = yx_core_queue_isEmpty(&(queue_ref->queue));
    yx_os_pthread_rwlock_unlock(&(queue_ref->rwlock));
    
    return isEmpty;
}
yx_size yx_core_resource_queue_count(yx_core_resource_queue_ref queue_ref)
{
    yx_size count;
    
    yx_os_pthread_rwlock_rlock(&(queue_ref->rwlock));
    count = yx_core_queue_count(&(queue_ref->queue));
    yx_os_pthread_rwlock_unlock(&(queue_ref->rwlock));
    
    return count;
}

/* fetch */
yx_value yx_core_resource_queue_first(yx_core_resource_queue_ref queue_ref)
{
    yx_value value;
    
    yx_os_pthread_rwlock_rlock(&(queue_ref->rwlock));
    value = yx_core_queue_first(&(queue_ref->queue));
    yx_os_pthread_rwlock_unlock(&(queue_ref->rwlock));
    
    return value;
}
yx_value yx_core_resource_queue_last(yx_core_resource_queue_ref queue_ref)
{
    yx_value value;
    
    yx_os_pthread_rwlock_rlock(&(queue_ref->rwlock));
    value = yx_core_queue_last(&(queue_ref->queue));
    yx_os_pthread_rwlock_unlock(&(queue_ref->rwlock));
    
    return value;

}

/* add & get */
void yx_core_resource_queue_add(yx_core_resource_queue_ref queue_ref, yx_value value)
{
    if (yx_false == queue_ref->running)
        return;
    
    yx_os_pthread_rwlock_wlock(&(queue_ref->rwlock));
    yx_core_queue_push(&(queue_ref->queue), value);
    yx_os_pthread_rwlock_unlock(&(queue_ref->rwlock));
    
    if (1 == yx_core_queue_count(&(queue_ref->queue))){
        
        yx_os_pthread_mutex_lock(&(queue_ref->mutex));
        yx_os_pthread_cond_signal(&(queue_ref->cond));
        yx_os_pthread_mutex_unlock(&(queue_ref->mutex));

    }
}

int yx_core_resource_queue_get(yx_core_resource_queue_ref queue_ref, yx_value* value, yx_real time)
{
    int rt = 0;
    *value = (yx_value)NULL;
    
    if (yx_false == queue_ref->running)
        return -1;
    
    yx_os_pthread_mutex_lock(&(queue_ref->mutex));
    
    if (queue_ref->running)
    {
        if(yx_core_resource_queue_isEmpty(queue_ref))
        {
            if (time > 0)
                rt = yx_os_pthread_cond_timewait(&(queue_ref->cond), &(queue_ref->mutex), time);
            else
                rt = yx_os_pthread_cond_wait(&(queue_ref->cond), &(queue_ref->mutex));
        }
        
        if (0 == rt)
        {
            yx_os_pthread_rwlock_wlock(&(queue_ref->rwlock));
            *value = yx_core_queue_first(&(queue_ref->queue));
            yx_core_queue_pop(&(queue_ref->queue));
            yx_os_pthread_rwlock_unlock(&(queue_ref->rwlock));
        }

    }
    
    yx_os_pthread_mutex_unlock(&(queue_ref->mutex));


    return rt;
}


void yx_core_resource_queue_stop(yx_core_resource_queue_ref queue_ref)
{
    queue_ref->running = yx_false;
    
    yx_os_pthread_mutex_lock(&(queue_ref->mutex));
    yx_os_pthread_cond_boarcast(&(queue_ref->cond));
    yx_os_pthread_mutex_unlock(&(queue_ref->mutex));

}



































































































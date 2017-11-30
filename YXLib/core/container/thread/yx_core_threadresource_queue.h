//
//  yx_core_threadresource_queue.h
//  YXCLib
//
//  Created by Yuxi Liu on 2017/11/30.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_THREADRESOURCE_QUEUE_H_
#define _YX_CORE_THREADRESOURCE_QUEUE_H_

#include "../yx_core_queue.h"


typedef struct yx_core_resource_queue{
    
    yx_core_queue queue;
    yx_os_thread_rwlock rwlock;
    
    yx_os_thread_mutex mutex;
    yx_os_thread_cond cond;
    
    yx_bool running;
    
    yx_allocator allocator;
    
}yx_core_resource_queue;


typedef struct yx_core_resource_queue* yx_core_resource_queue_ref;


/* create & destroy */
yx_core_resource_queue_ref yx_core_resource_queue_create(yx_allocator allocator);
void yx_core_resource_queue_init(yx_allocator allocator, yx_core_resource_queue_ref queue_ref);

void yx_core_resource_queue_recycle(yx_core_resource_queue_ref queue_ref);
void yx_core_resource_queue_destroy(yx_core_resource_queue_ref* queue_ref_ptr);

/* property */
yx_bool yx_core_resource_queue_isEmpty(yx_core_resource_queue_ref queue_ref);
yx_size yx_core_resource_queue_count(yx_core_resource_queue_ref queue_ref);

/* fetch */
yx_value yx_core_resource_queue_first(yx_core_resource_queue_ref queue_ref);
yx_value yx_core_resource_queue_last(yx_core_resource_queue_ref queue_ref);

/* add & get */
void yx_core_resource_queue_add(yx_core_resource_queue_ref queue_ref, yx_value value);
int yx_core_resource_queue_get(yx_core_resource_queue_ref queue_ref, yx_value* value, yx_real time); //time 如果小于0， 则一直等待

/*stop*/
void yx_core_resource_queue_stop(yx_core_resource_queue_ref queue_ref);


#endif /* _YX_CORE_THREADRESOURCE_QUEUE_H_ */











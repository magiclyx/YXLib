//
//  yx_core_tsmembucket.c
//  YXCLib
//
//  Created by Yuxi Liu on 2017/12/3.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#include "yx_core_tsmembucket.h"
#include "../yx_core_membucket.h"
#include "../../debug/yx_core_assert.h"
#include "../../rttidef/yx_core_rttidef.h"

#define HANDLE2WRAPPER(handle) ((_ts_membucket_wrapper_ref)handle)

typedef struct _ts_membucket_wrapper{
    
    YX_ALLOCATOR_STRUCT_DECLARE;

    yx_allocator allocator;
    
    yx_allocator membucket;
    yx_os_spinlock spinlock;
    
}_ts_membucket_wrapper;

typedef struct _ts_membucket_wrapper* _ts_membucket_wrapper_ref;



void* _yx_tscellMempoo_std_memalign(yx_allocator allocator, yx_size alignment, yx_size size);
yx_ptr _yx_tscellMempool_std_alloc(yx_allocator allocator, yx_size size);




yx_allocator yx_tsmembucket_create(yx_allocator parentpool, yx_size cellSize)
{
    /*
     如果不是稳定内存池，则递归父内存池，找到一个稳定的内存池
     */
    if (NULL == parentpool)
        parentpool = yx_get_allocator();
    
    
    yx_allocator membucket = yx_membucket_create(parentpool, cellSize);
    if (yx_unlikely( !membucket )) {
        return NULL;
    }
    
    yx_os_spinlock spinlock = YX_OS_SPINLOCK_INIT;
    
    
    _ts_membucket_wrapper_ref threadsafe_wrapper = yx_allocator_alloc(parentpool, sizeof(struct _ts_membucket_wrapper));
    
    threadsafe_wrapper->allocator = parentpool;
    threadsafe_wrapper->membucket = membucket;
    threadsafe_wrapper->spinlock = spinlock;
    
    
    yx_allocator_setup(yx_rtti_allocator_ts_memsection,
                       threadsafe_wrapper,
                       _yx_tscellMempool_std_alloc,
                       _yx_tscellMempoo_std_memalign,
                       yx_tsmembucket_free);
    
    
    return (yx_allocator)threadsafe_wrapper;
    
    
errout:
    
    if (NULL == membucket)
        yx_membucket_destroy(&membucket);
    
    if (NULL != threadsafe_wrapper)
        yx_allocator_free(parentpool, threadsafe_wrapper);
    
    return NULL;

}

void yx_tsmembucket_destroy(yx_allocator* allocator_ref)
{
    _ts_membucket_wrapper_ref wrapper = HANDLE2WRAPPER(*allocator_ref);
    *allocator_ref = NULL;
    
    yx_os_spin_lock(&(wrapper->spinlock));
    yx_membucket_destroy(&(wrapper->membucket));
    yx_os_spin_unlock(&(wrapper->spinlock));
    
    yx_allocator_free(wrapper->allocator, wrapper);
}

yx_ptr yx_tsmembucket_alloc(yx_allocator allocator)
{
    _ts_membucket_wrapper_ref wrapper = HANDLE2WRAPPER(allocator);
    
    yx_ptr ptr;
    yx_os_spin_lock(&(wrapper->spinlock));
    ptr = yx_membucket_alloc(wrapper->membucket);
    yx_os_spin_unlock(&(wrapper->spinlock));

    return ptr;
}

void yx_tsmembucket_free(yx_allocator allocator, yx_ptr ptr)
{
    _ts_membucket_wrapper_ref wrapper = HANDLE2WRAPPER(allocator);

    yx_os_spin_lock(&(wrapper->spinlock));
    yx_membucket_free(wrapper->membucket, ptr);
    yx_os_spin_unlock(&(wrapper->spinlock));
}


/*get parent allocator*/
yx_allocator yx_tsmembucket_parentpool(yx_allocator allocator)
{
    _ts_membucket_wrapper_ref wrapper = HANDLE2WRAPPER(allocator);
    
    return wrapper->allocator;
}

#pragma mark private
void* _yx_tscellMempoo_std_memalign(yx_allocator allocator, yx_size alignment, yx_size size)
{
    yx_unused(allocator);
    yx_unused(alignment);
    yx_unused(size);
    
    /*why you get here ?!!*/
    YX_ASSERT(0);
    
    return NULL;
}

yx_ptr _yx_tscellMempool_std_alloc(yx_allocator allocator, yx_size size)
{
    yx_unused(size);
    return yx_tsmembucket_alloc(allocator);
}


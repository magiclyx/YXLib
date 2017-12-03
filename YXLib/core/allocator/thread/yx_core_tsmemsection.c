//
//  yx_core_tsmemsection.c
//  YXCLib
//
//  Created by Yuxi Liu on 2017/12/2.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#include "yx_core_tsmemsection.h"
#include "../yx_core_memsection.h"
#include "../../rttidef/yx_core_rttidef.h"
#include "../yx_core_membucket.h"
#include "../../debug/yx_core_assert.h"
#include "yx_core_tsmembucket.h"

#define HANDLE2WRAPPER(handle) ((_ts_memsection_wrapper_ref)handle)


typedef struct _ts_memsection_wrapper{
    YX_ALLOCATOR_STRUCT_DECLARE;
    
    yx_allocator allocator;
    yx_allocator memsection;
    yx_os_spinlock spinlock;
}_ts_memsection_wrapper;

typedef struct _ts_memsection_wrapper* _ts_memsection_wrapper_ref;


static yx_allocator _stableAllocator(yx_allocator allocator);



yx_allocator yx_tsmemsection_create(yx_allocator allocator, yx_size size){
    
    /*
     如果不是稳定内存池，则递归父内存池，找到一个稳定的内存池
     */
    allocator = _stableAllocator(allocator);
    
    
    yx_allocator memsection = yx_memsection_create(allocator, size);
    if (yx_unlikely( !memsection )) {
        return NULL;
    }
    
    yx_os_spinlock spinlock = YX_OS_SPINLOCK_INIT;

    
    _ts_memsection_wrapper_ref threadsafe_wrapper = yx_allocator_alloc(allocator, sizeof(struct _ts_memsection_wrapper));
    
    threadsafe_wrapper->allocator = allocator;
    threadsafe_wrapper->memsection = memsection;
    threadsafe_wrapper->spinlock = spinlock;
    
    
    yx_allocator_setup(yx_rtti_allocator_ts_memsection,
                       threadsafe_wrapper,
                       yx_tsmemsection_alloc,
                       yx_tsmemsection_memalign,
                       yx_tsmemsection_freelarge);

    
    return (yx_allocator)threadsafe_wrapper;

    
errout:
    
    if (NULL == memsection)
        yx_memsection_destroy(&memsection);
    
    if (NULL != threadsafe_wrapper)
        yx_allocator_free(allocator, threadsafe_wrapper);
    
    return NULL;
}


void yx_tsmemsection_destroy(yx_allocator* allocator_ref){
    
    _ts_memsection_wrapper_ref wrapper = HANDLE2WRAPPER(*allocator_ref);
    *allocator_ref = NULL;
    
    yx_os_spin_lock(&(wrapper->spinlock));
    yx_memsection_destroy(&(wrapper->memsection));
    yx_os_spin_unlock(&(wrapper->spinlock));
    
    
    yx_allocator_free(wrapper->allocator, wrapper);
}




void* yx_tsmemsection_alloc(yx_allocator allocator, yx_size size){
    _ts_memsection_wrapper_ref wrapper = HANDLE2WRAPPER(allocator);
    
    void* ptr;
    yx_os_spin_lock(&(wrapper->spinlock));
    ptr = yx_memsection_alloc(wrapper->memsection, size);
    yx_os_spin_unlock(&(wrapper->spinlock));

    return ptr;
}

void* yx_tsmemsection_memalign(yx_allocator allocator, yx_size alignment, yx_size size){
    _ts_memsection_wrapper_ref wrapper = HANDLE2WRAPPER(allocator);
    
    void* ptr;
    yx_os_spin_lock(&(wrapper->spinlock));
    ptr = yx_memsection_memalign(wrapper->memsection, alignment, size);
    yx_os_spin_unlock(&(wrapper->spinlock));

    return ptr;
}

void yx_tsmemsection_freelarge(yx_allocator allocator, yx_ptr ptr){
    _ts_memsection_wrapper_ref wrapper = HANDLE2WRAPPER(allocator);
    
    yx_os_spin_lock(&(wrapper->spinlock));
    yx_memsection_freelarge(wrapper->memsection, ptr);
    yx_os_spin_unlock(&(wrapper->spinlock));
}

void yx_tsmemsection_reset(yx_allocator allocator){
    _ts_memsection_wrapper_ref wrapper = HANDLE2WRAPPER(allocator);

    yx_os_spin_lock(&(wrapper->spinlock));
    yx_memsection_reset(wrapper->memsection);
    yx_os_spin_unlock(&(wrapper->spinlock));

}

yx_allocator yx_tsmemsection_parentpool(yx_allocator allocator)
{
    _ts_memsection_wrapper_ref wraper = HANDLE2WRAPPER(allocator);
    
    return wraper->allocator;
}



#pragma mark private

static yx_allocator _stableAllocator(yx_allocator allocator)
{
    for (;;)
    {
        switch (yx_os_rtti_val(allocator))
        {
            case yx_rtti_allocator_ts_membucket:
                allocator = yx_tsmembucket_parentpool(allocator);
                break;
            case yx_rtti_allocator_ts_memsection:
                allocator = yx_tsmemsection_parentpool(allocator);
                break;
            case yx_rtti_allocator_memsection:
                allocator = yx_memsection_parentpool(allocator);
                break;
            case yx_rtti_allocator_membucket:
                allocator = yx_membucket_parentpool(allocator);
                break;
            case yx_rtti_allocator_stdC:
            case yx_rtti_core_base:
            case yx_rtti_allocator_debug:
                return allocator;
                
            default:
                YX_ASSERT(0); //unknown allocator
                break;
        }
    }
}





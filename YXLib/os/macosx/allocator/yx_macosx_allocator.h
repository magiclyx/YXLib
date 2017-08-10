//
//  yx_macosx_allocator.h
//  YXLib
//
//  Created by Yuxi Liu on 9/25/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_ALLOCATOR_H_
#define _YX_MACOSX_ALLOCATOR_H_ 

#include "../basictypes/yx_macosx_types.h"
#include "../rtti/yx_macosx_rtti.h"


typedef yx_handle yx_allocator;

typedef struct _yx_allocator_context{
    yx_rtti rtti;
    yx_allocator handle;
    yx_ptr(*alloc)(yx_allocator allocator, yx_size size);
    yx_ptr(*memalign)(yx_allocator allocator, yx_size alignment, yx_size size);
    void(*free)(yx_allocator allocator, yx_ptr ptr);
}yx_allocator_context, *yx_allocator_contex_ref;


#pragma mark - setup allocator


/**
 *  allocator defined
 */

#define YX_ALLOCATOR_STRUCT_DECLARE const yx_allocator_context __self__allocator__


/**
 *  setup an allocator
 */
#define yx_allocator_setup(rtti_val, allocator, alloc_fun, memalign_fun, free_fun) \
(*((yx_rtti_ref)(allocator))) = rtti_val; \
((yx_allocator_contex_ref)(allocator))->handle = allocator; \
((yx_allocator_contex_ref)(allocator))->alloc = alloc_fun; \
((yx_allocator_contex_ref)(allocator))->memalign = memalign_fun; \
((yx_allocator_contex_ref)(allocator))->free = free_fun




#pragma mark - alloc from allocator

/*alloc memory with the allocator*/
#define yx_allocator_alloc(allocater, size) (((yx_allocator_contex_ref)allocater)->alloc(((yx_allocator_contex_ref)allocater)->handle, size))

/*free memory with the allocator*/
#define yx_allocator_free(allocater, ptr) ( ((yx_allocator_contex_ref)allocater)->free(((yx_allocator_contex_ref)allocater)->handle, ptr) )

#define yx_allocator_memalign(allocator, aligment, size) (((yx_allocator_contex_ref)allocator)->memalign(((yx_allocator_contex_ref)allocator)->handle, aligment, size))


#pragma mark - global allocator

extern yx_allocator global_allocator;
extern yx_allocator persistent_allocator;


/*std C allocator*/
yx_allocator yx_stdC_allocator(void);




/*
 global allocator
 
 TODO: add push allocator and pop allocator
 
 这里所有set的allocator应该具备完整的申请释放功能
 */

static yx_inline yx_allocator yx_get_allocator()
{
    if (yx_likely( global_allocator )) {
        return global_allocator;
    }
    
    return (global_allocator = yx_stdC_allocator());
}

static yx_inline yx_allocator yx_set_allocator(yx_allocator allocator){
    yx_allocator old_allocator = global_allocator;
    global_allocator = allocator;
    
    return old_allocator;
}


/*
 persistent allocator
 
 这里用于保存程序启动后申请并长久持有的内存块，这些内存块的生命周期几乎等于整个程序的生命周期
 */
//persistent_allocator

static yx_inline yx_allocator yx_get_persistent_allocator()
{
    if (yx_likely( persistent_allocator )) {
        return persistent_allocator;
    }
    
    return (persistent_allocator = yx_stdC_allocator());
}


static yx_inline yx_allocator yx_set_persistent_allocator(yx_allocator allocator){
    yx_allocator old_allocator = global_allocator;
    global_allocator = allocator;
    
    return old_allocator;
}



#endif



//
//  yx_unittest_mempool.c
//  YXLib
//
//  Created by Yuxi Liu on 1/5/15.
//  Copyright (c) 2015 Yuxi Liu. All rights reserved.
//

#include "yx_unittest_mempool.h"

#include "yx_unittest_rttidef.h"
#include "yx_unittest_list.h"


struct __yx_unittest_mempool_session_head
{
    YX_STRUCT_ALIGNMENT_INFILLING(sizeof(yx_uint32));
    yx_uint32 head_guid;
};

struct __yx_unittest_mempool_session_tail
{
    yx_uint32 tail_guid;
    YX_STRUCT_ALIGNMENT_INFILLING(sizeof(yx_uint32));
};


struct __yx_unittest_mempool_node_s_
{
    void* mem;
};

struct __yx_unittest_mempool_context_s_
{
    YX_ALLOCATOR_STRUCT_DECLARE;
    yx_allocator allocator;
    
    yx_size mem_size;
    _yx_ut_list mem_list;
    _yx_ut_list empty_list;
};


#pragma mark declare
////////////////////////////////////////////////////////////////////////////////////////////////////
/*declare*/
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _expand_free_list(struct __yx_unittest_mempool_context_s_* context, yx_uint expandNum);
static void _memoryNode_destroy_callback(_yx_ut_list list, yx_handle handle, yx_index index);
static void _emptyNode_destroy_callback(_yx_ut_list list, yx_handle handle, yx_index index);



#pragma mark public
////////////////////////////////////////////////////////////////////////////////////////////////////
/*public*/
////////////////////////////////////////////////////////////////////////////////////////////////////

yx_allocator yx_unittest_mempool_create(yx_uint blockNum, yx_allocator allocator)
{
    struct __yx_unittest_mempool_context_s_* context = NULL;
    
    if (NULL == allocator)
        allocator = yx_get_allocator();
    
    context = yx_allocator_alloc(allocator, sizeof(struct __yx_unittest_mempool_context_s_));
    if (NULL == context)
        goto errout;
    
    /*mem list*/
    context->mem_list = _yx_ut_list_create(allocator);
    if (NULL == context->mem_list)
        goto errout;
    
    /*free list*/
    context->empty_list = _yx_ut_list_create(allocator);
    if (NULL == context->empty_list)
        goto errout;
    
    /*binding the context to all list*/
    _yx_ut_list_bindingData_set(context->mem_list, context);
    _yx_ut_list_bindingData_set(context->empty_list, context);
    
    
    /*mem size*/
    context->mem_size = 0;
    
    
    /*setup the allocator*/
    context->allocator = allocator;
    yx_allocator_setup(yx_rtti_allocator_unittest, context, yx_unittest_mempool_alloc, NULL, yx_unittest_mempool_free);
    
    
    /*add free node into empty_list*/
    _expand_free_list(context, blockNum);
    
    
    return (yx_allocator)context;
    
errout:
   
    if (NULL != context) {
        if (NULL != context->mem_list)
            _yx_ut_list_destroy(&(context->mem_list), NULL);
        
        if (NULL != context->empty_list)
            _yx_ut_list_destroy(&(context->empty_list), NULL);
        
        yx_allocator_free(allocator, context);
    }
    
    return NULL;
}

void yx_unittest_mempool_destroy(yx_allocator* allocator_ptr)
{
}

void* yx_unittest_mempool_alloc(yx_allocator allocator, yx_size size)
{
    return NULL;
}
void yx_unittest_mempool_free(yx_allocator allocator, yx_ptr address)
{
}

#pragma mark private
////////////////////////////////////////////////////////////////////////////////////////////////////
/*private*/
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _expand_free_list(struct __yx_unittest_mempool_context_s_* context, yx_uint expandNum)
{
    yx_allocator allocator = context->allocator;
    _yx_ut_list freeList = context->empty_list;
    for (yx_uint i = 0; i < expandNum; i++) {
        struct __yx_unittest_mempool_node_s_* node = yx_allocator_alloc(allocator, sizeof(struct __yx_unittest_mempool_node_s_));
        
        if (NULL == node)
            break;
        
        node->mem = NULL;
        _yx_ut_list_insert_back(freeList, node);
    }
}

static void _emptyNode_destroy_callback(_yx_ut_list list, yx_handle handle, yx_index index)
{
    
    yx_unused(index);
    
    struct __yx_unittest_mempool_node_s_* node = (struct __yx_unittest_mempool_node_s_*)handle;
    struct __yx_unittest_mempool_context_s_* context = _yx_ut_list_bindingData_get(list);
    
    YX_ASSERT(NULL != node);
    YX_ASSERT(NULL != context);
    
    YX_ASSERT(NULL == node->mem); //the memory must be released
    yx_allocator_free(context->allocator, node);
}

static void _memoryNode_destroy_callback(_yx_ut_list list, yx_handle handle, yx_index index)
{
    yx_unused(index);
    
    struct __yx_unittest_mempool_node_s_* node = (struct __yx_unittest_mempool_node_s_*)handle;
    struct __yx_unittest_mempool_context_s_* context = _yx_ut_list_bindingData_get(list);
    
    YX_ASSERT(NULL != node);
    YX_ASSERT(NULL != context);
    
    YX_ASSERT(NULL != node->mem); //the memory must not be released
    
    yx_allocator_free(context->allocator, node->mem);
    yx_allocator_free(context->allocator, node);
}





//
//  yx_core_mempool_thread.c
//  YXLib
//
//  Created by LiuYuxi on 15/7/6.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_mempool_thread.h"

#include "../container/yx_core_listnode.h"
#include "../container/yx_core_stacknode.h"
#include "../rttidef/yx_core_rttidef.h"

#define HANDLE2THREADMEMPOOL(handle) ((yx_core_threadpool_context*)handle)

typedef struct _yx_core_threadpool_mempiece_
{
    struct yx_core_listnode node;
}yx_core_threadpool_mempiece;

typedef struct _yx_core_threadpool_block_
{
    struct yx_core_listnode node;
    void* last;
    void* end;
    yx_value failed;
    
}yx_core_threadpool_block;

typedef struct _yx_core_threadpool_pool_
{
    YX_ALLOCATOR_STRUCT_DECLARE;

    yx_core_threadpool_block* current;//48
    
    struct yx_core_listnode node; /*using for pool stack in context*/
    struct yx_core_listnode block_node ; /*using for storage all blocks in mempool*/
    
    struct yx_core_listnode largeblock_list;
    struct yx_core_listnode empty_largeblock_list; //can remove this and accessory the reset function
    
}yx_core_threadpool_pool;


typedef struct _yx_core_threadpool_large_block_
{
    struct yx_core_listnode node;
    void* mem;
}yx_core_threadpool_large_block;

typedef struct _yx_core_threadpool_context_
{
    yx_allocator allocator;
    
    struct yx_core_listnode block_list; //list of _yx_core_threadpool_head_
    struct yx_core_listnode pool_stack; //list of _yx_core_threadpool_pool_
    
    yx_size max_size;

}yx_core_threadpool_context;


static yx_size _maxsize_of_buf()
{
    /*
     * for basic pool
     * NGX_MAX_ALLOC_FROM_POOL should be (ngx_pagesize - YX_ALIGNMENT).
     * On Windows NT it decreases a number of locked pages in a kernel.
     *
     */
    return (yx_size)yx_os_system_maxpagesize() - YX_ALIGNMENT;
}

static yx_core_threadpool_head* _create_new_buf(yx_core_threadpool_context* context)
{
    
    yx_size size = context->max_size;
    yx_core_threadpool_head* buf_head = yx_allocator_alloc(context->allocator, sizeof(yx_core_threadpool_head) + size);
    
    if(yx_unlikely( !buf_head )) {
        return NULL;
    }
    
    buf_head->last = (yx_ptr)(buf_head) + sizeof(yx_core_threadpool_head);
    buf_head->end = (yx_ptr)(buf_head->last) + size;
    buf_head->failed = 0;
    
    
    yx_core_listnode_insertTail(&(context->block_list), &(buf_head->node));
    
    
    return buf_head;
}

static void _yx_threadMempool_block_failed(yx_core_threadpool_context* context)
{
    yx_core_threadpool_head* buf_head = context->current;
    
    switch(++(buf_head->failed))
    {
        case 0:
        case 1:
            if (buf_head->end - buf_head->last < YX_ALIGNMENT)
            {
                struct yx_core_listnode* next_node = yx_core_listnode_nextNode(&(context->current->node));
                yx_core_listnode_swap(&(buf_head->node), &(context->current->node));
                
                if (next_node == &(context->block_list)) {
                    context->current = NULL;
                }
                else {
                    context->current = yx_core_listnode_owner(next_node, yx_core_threadpool_head, node);
                }
                
                break;
            }
        case 2:
            if (buf_head->end - buf_head->last < 2 * YX_ALIGNMENT)
            {
                struct yx_core_listnode* next_node = yx_core_listnode_nextNode(&(context->current->node));
                yx_core_listnode_swap(&(buf_head->node), &(context->current->node));
                
                if (next_node == &(context->block_list)) {
                    context->current = NULL;
                }
                else {
                    context->current = yx_core_listnode_owner(next_node, yx_core_threadpool_head, node);
                }
                
                break;
            }
            
        case 3:
            /* try one more time in 3* YX_ALIGNMENT */
        case 4:
            if (buf_head->end - buf_head->last < 3 * YX_ALIGNMENT)
            {
                struct yx_core_listnode* next_node = yx_core_listnode_nextNode(&(context->current->node));
                yx_core_listnode_swap(&(buf_head->node), &(context->current->node));
                
                if (next_node == &(context->block_list)) {
                    context->current = NULL;
                }
                else {
                    context->current = yx_core_listnode_owner(next_node, yx_core_threadpool_head, node);
                }
                
                break;
            }
            
        default:
            /*buf_head->failed > 4*/
        {
            struct yx_core_listnode* next_node = yx_core_listnode_nextNode(&(buf_head->node));
            if (next_node == &(context->block_list)) {
                context->current = NULL;
            }
            else {
                context->current = yx_core_listnode_owner(next_node, yx_core_threadpool_head, node);
            }
            
            break;
        }
    } //end of switch
    
}

yx_threadpool yx_threadMempool_create(yx_allocator parentPool, yx_size size)
{
    if (NULL == parentPool) {
        parentPool = yx_get_allocator();
    }
    
    
    /*create the context*/
    yx_core_threadpool_context* context = yx_allocator_alloc(parentPool, sizeof(yx_core_threadpool_context));
    if (yx_unlikely( !context )) {
        return NULL;
    }
    
    context->max_size = size;
    context->allocator = parentPool;
    yx_core_listnode_sentinelInit(&(context->block_list));
    yx_core_listnode_sentinelInit(&(context->pool_stack));
    
    
    
    yx_allocator_setup(yx_rtti_allocator_thread,
                       context,
                       yx_threadMempool_alloc,
                       yx_threadMempool_memalign,
                       yx_threadMempool_free);
    
    
    return (yx_allocator)context;
}
void yx_threadMempool_destroy(yx_threadpool* threadpool_ref)
{
//    yx_core_threadpool_context* context = HANDLE2THREADMEMPOOL(*threadpool_ref);
//    *threadpool_ref = NULL;
//    
//    /*release large block*/
//    yx_core_threadpool_large_block* block;
//    yx_core_threadpool_large_block* tmp_block;
//    yx_core_listnode_ownerForEachSafe(block, tmp_block, &(context->largeblock_list), node)
//    {
//        yx_allocator_free(context->allocator, block->mem);
//    }
//    
//    
//    /*reset all buf*/
//    yx_core_threadpool_head* buf_head;
//    yx_core_threadpool_head* tmp_head;
//    yx_core_listnode_ownerForEachSafe(buf_head, tmp_head, &(context->block_list), node)
//    {
//        yx_core_listnode_del(&(buf_head->node));
//        yx_allocator_free(context->allocator, buf_head);
//    }
//    
//    yx_allocator_free(context->allocator, context);
}

yx_allocator yx_threadMempool_push(yx_threadpool threadPool)
{
    yx_core_threadpool_context* context = HANDLE2THREADMEMPOOL(threadPool);
    yx_core_threadpool_pool* new_pool;
    yx_core_threadpool_head* new_buf = NULL;
    
    if (yx_core_listnode_isEmpty( &(context->pool_stack) ))
    {
        if (yx_core_listnode_isEmpty( &(context->block_list) ))
        {
            yx_allocator_alloc(context->allocator, sizeof());
        }
    }
    else
    {
        yx_core_threadpool_pool* prev_pool = yx_core_listnode_firstNodeOwner(&(context->pool_stack), yx_core_threadpool_pool, node);
    }
    yx_core_threadpool_head* buf_head = context->current;
    yx_core_threadpool_head* new_buf = NULL;
    
    if (NULL == buf_head)
    {
        new_buf = _create_new_buf(context);
        context->current = new_buf;
        
    }
    else if(buf_head->end - buf_head->last >= (yx_long)sizeof(yx_core_threadpool_head))
    {
        new_buf = buf_head->last;
    }
    else
    {
        _yx_threadMempool_block_failed(context);
        
        new_buf = _create_new_buf(context);
        
        /*_yx_threadMempool_block_failed 可能会重置 current指针，因此不能用buf_head*/
        if (NULL == context->current)
            context->current = new_buf;
    }
    
    new_buf->end = context->current->end;
    new_buf->last = context->current->last + sizeof(yx_core_threadpool_head);
    new_buf->failed = context->current->failed;
    
    return (yx_allocator)new_buf;
}



void yx_threadMempool_pop(yx_threadpool threadPool);

void* yx_threadMempool_alloc(yx_allocator allocator, yx_size size);
void* yx_threadMempool_memalign(yx_allocator allocator, yx_size alignment, yx_size size);
void yx_threadMempool_free(yx_allocator allocator, yx_ptr ptr);

//
//  yx_core_mempool_buf.c
//  YXLib
//
//  Created by LiuYuxi on 15/4/17.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_memsection.h"
#include "yx_core_mem.h"

#include "../container/yx_core_listnode.h"
#include "../rttidef/yx_core_rttidef.h"
#include "../debug/yx_core_assert.h"
#include "thread/yx_core_tsmemsection.h"
#include "thread/yx_core_tsmembucket.h"
#include "yx_core_membucket.h"

#define HANDLE2BUFMEMPOOL(handle) ((yx_core_mempool_context*)handle)



typedef struct _yx_core_mempool_buf_head_
{
    struct yx_core_listnode node;
    void* last;
    void* end;
    yx_value failed;
}yx_core_mempool_buf_head;

YX_STATIC_ASSERT_ALIGMENT(struct _yx_core_mempool_buf_head_);


typedef struct _yx_core_mempool_large_block
{
    struct yx_core_listnode node;
    void* mem;
}yx_core_mempool_large_block;


typedef struct _yx_core_mempool_context_
{
    YX_ALLOCATOR_STRUCT_DECLARE;
    yx_allocator allocator;
    
    yx_size max_size;
    
    struct _yx_core_mempool_buf_head_* current;
    struct yx_core_listnode buf_list;
    
    struct yx_core_listnode largeblock_list;
    struct yx_core_listnode empty_largeblock_list; //can remove this and accessory the reset function
}yx_core_mempool_context;


////////////////////////////////////////////////////////////////////////////////////////////////////

static yx_core_mempool_buf_head* _create_new_buf(yx_core_mempool_context* context);
static void* _alloc_from_base_pool(yx_core_mempool_context* context, yx_size size);
static yx_inline yx_size _maxsize_of_buf();
static yx_allocator _stableAllocator(yx_allocator allocator);

////////////////////////////////////////////////////////////////////////////////////////////////////
yx_allocator yx_memsection_create(yx_allocator allocator, yx_size size)
{
    if (NULL == allocator) {
        allocator = yx_get_allocator();
    }
    
    /*
     如果不是稳定内存池，则递归父内存池，找到一个稳定的内存池
     */
    allocator = _stableAllocator(allocator);
    
    
    /*create the context*/
    yx_core_mempool_context* context = yx_allocator_alloc(allocator, sizeof(yx_core_mempool_context));
    if (yx_unlikely( !context )) {
        return NULL;
    }
    
    yx_size max_page_size = _maxsize_of_buf();
    size = (size < max_page_size)? size: max_page_size;
    
    context->max_size = size;
    context->allocator = allocator;
    yx_core_listnode_sentinelInit(&(context->buf_list));
    yx_core_listnode_sentinelInit(&(context->largeblock_list));
    yx_core_listnode_sentinelInit(&(context->empty_largeblock_list));
    
    
    
    /*create the first node*/
    yx_core_mempool_buf_head* first_node = _create_new_buf(context);
    context->current = first_node;
    
    
    
    
    yx_allocator_setup(yx_rtti_allocator_memsection,
                       context,
                       yx_memsection_alloc,
                       yx_memsection_memalign,
                       yx_memsection_freelarge);
    
    return (yx_allocator)context;
}

void yx_memsection_destroy(yx_allocator* allocator_ref)
{
    yx_core_mempool_context* context = HANDLE2BUFMEMPOOL(*allocator_ref);
    *allocator_ref = NULL;
    
    /*release large block*/
    yx_core_mempool_large_block* block;
    yx_core_mempool_large_block* tmp_block;
    yx_core_listnode_ownerForEachSafe(block, tmp_block, &(context->largeblock_list), node)
    {
        yx_allocator_free(context->allocator, block->mem);
    }
    
    
    /*reset all buf*/
    yx_core_mempool_buf_head* buf_head;
    yx_core_mempool_buf_head* tmp_head;
    yx_core_listnode_ownerForEachSafe(buf_head, tmp_head, &(context->buf_list), node)
    {
        yx_core_listnode_del(&(buf_head->node));
        yx_allocator_free(context->allocator, buf_head);
    }
    
    yx_allocator_free(context->allocator, context);
    
}



void* yx_memsection_alloc(yx_allocator allocator, yx_size size)
{
    yx_core_mempool_context* context = HANDLE2BUFMEMPOOL(allocator);
    
    /*alignment the size*/
    size = YX_UINT64_ALIGNMENT(size);
    
    void* mem;
    
    if(yx_likely( size <= context->max_size ))
    {
        yx_core_mempool_buf_head* buf_head = context->current;
        if (NULL != buf_head)
        {
            yx_core_listnode_ownerForEachFrom(buf_head, &(context->buf_list), node)
            {
                
                if (buf_head->end - buf_head->last >= (yx_long)size)
                {
                    mem = buf_head->last;
                    buf_head->last = (yx_ptr)(buf_head->last) + size;
                    
                    return mem;
                }
                
                
                switch(++(buf_head->failed))
                {
                    case 0:
                    case 1:
                        if (buf_head->end - buf_head->last < YX_ALIGNMENT)
                        {
                            struct yx_core_listnode* next_node = yx_core_listnode_nextNode(&(context->current->node));
                            yx_core_listnode_swap(&(buf_head->node), &(context->current->node));
                            
                            if (next_node == &(context->buf_list)) {
                                context->current = NULL;
                            }
                            else {
                                context->current = yx_core_listnode_owner(next_node, yx_core_mempool_buf_head, node);
                            }

                            break;
                        }
                    case 2:
                        if (buf_head->end - buf_head->last < 2 * YX_ALIGNMENT)
                        {
                            struct yx_core_listnode* next_node = yx_core_listnode_nextNode(&(context->current->node));
                            yx_core_listnode_swap(&(buf_head->node), &(context->current->node));
                            
                            if (next_node == &(context->buf_list)) {
                                context->current = NULL;
                            }
                            else {
                                context->current = yx_core_listnode_owner(next_node, yx_core_mempool_buf_head, node);
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
                            
                            if (next_node == &(context->buf_list)) {
                                context->current = NULL;
                            }
                            else {
                                context->current = yx_core_listnode_owner(next_node, yx_core_mempool_buf_head, node);
                            }
                            
                            break;
                        }

                    default:
                        /*buf_head->failed > 4*/
                        {
                            struct yx_core_listnode* next_node = yx_core_listnode_nextNode(&(buf_head->node));
                            if (next_node == &(context->buf_list)) {
                                context->current = NULL;
                            }
                            else {
                                context->current = yx_core_listnode_owner(next_node, yx_core_mempool_buf_head, node);
                            }
                            
                            break;
                        }
                } //end of switch
                
            } //end of list walking
            
        } //end of NULL = buf_head
        
        yx_core_mempool_buf_head* new_buf = _create_new_buf(context);
        mem = new_buf->last;
        new_buf->last += size;
        
        if (NULL == context->current) {
            context->current = new_buf;
        }
        
        
        return mem;

    }
    
    
    return _alloc_from_base_pool(context, size);
}

void* yx_memsection_memalign(yx_allocator allocator, yx_size alignment, yx_size size)
{
    if (alignment == YX_ALIGNMENT)
    {
        return yx_memsection_alloc(allocator, size);
    }
    
    
    yx_core_mempool_context* context = HANDLE2BUFMEMPOOL(allocator);
    void* mem;
    
    yx_size guess_size = YX_UINT64_ALIGNMENT(size + 2 * alignment);
    if (guess_size < context->max_size)
    {
        mem = yx_memsection_alloc(allocator, guess_size);
        const yx_size offset = alignment - (((yx_size)mem) % alignment);
        mem += offset;
        
        return mem;
    }
    else
    {
        mem = yx_allocator_memalign(context->allocator, alignment, size);
        if ( NULL == mem ) {
            return NULL;
        }
        
        yx_core_mempool_large_block* largeBlock;
        if (yx_core_listnode_isEmpty( &(context->empty_largeblock_list) ))
        {
            largeBlock = yx_memsection_alloc(context, sizeof(yx_core_mempool_large_block));
            largeBlock->mem = mem;
            yx_core_listnode_insertHead(&(context->largeblock_list), &(largeBlock->node));
        }
        else
        {
            largeBlock = yx_core_listnode_firstNodeOwner(&(context->empty_largeblock_list), yx_core_mempool_large_block, node);
            yx_core_listnode_del(&(largeBlock->node));
            largeBlock->mem = mem;
            yx_core_listnode_insertHead(&(context->largeblock_list), &(largeBlock->node));
        }
        
        
        
        return mem;
    }
    
}


void yx_memsection_freelarge(yx_allocator allocator, yx_ptr ptr)
{
    
    yx_core_mempool_context* context = HANDLE2BUFMEMPOOL(allocator);

    yx_core_mempool_large_block* block;
    yx_core_mempool_large_block* tmp_block;
    yx_core_listnode_ownerForEachSafe(block, tmp_block, &(context->largeblock_list), node)
    {
        if (ptr == block->mem)
        {
            yx_core_listnode_del( &(block->node) );
            yx_allocator_free(context->allocator, ptr);
            yx_core_listnode_insertHead(&(context->empty_largeblock_list), &(block->node));
            
            break;
        }
    }
}



void yx_memsection_reset(yx_allocator allocator)
{
    yx_core_mempool_context* context = HANDLE2BUFMEMPOOL(allocator);
    
    
    /*reset all buf*/
    yx_core_mempool_buf_head* buf_head;
    yx_core_listnode_ownerForEach(buf_head, &(context->buf_list), node)
    {
        buf_head->last = (yx_ptr)buf_head + sizeof(yx_core_mempool_buf_head);
        buf_head->failed = 0;
    }
    
    /*reset the current buf*/
    context->current = yx_core_listnode_firstNodeOwner(&(context->buf_list),
                                                       yx_core_mempool_buf_head,
                                                       node);
    
    
    /*reset large block*/
    yx_core_mempool_large_block* block;
    yx_core_mempool_large_block* tmp_block;
    yx_core_listnode_ownerForEachSafe(block, tmp_block, &(context->largeblock_list), node)
    {
        yx_core_listnode_del(&(block->node));
        yx_allocator_free(context->allocator, block->mem);
        yx_core_listnode_insertHead(&(context->empty_largeblock_list), &(block->node));
    }
}

/*get parent allocator*/
yx_allocator yx_memsection_parentpool(yx_allocator allocator)
{
    yx_core_mempool_context* context = HANDLE2BUFMEMPOOL(allocator);
    
    return context->allocator;
}


yx_int yx_memsection_blocknum(yx_allocator allocator)
{
    yx_core_mempool_context* context = HANDLE2BUFMEMPOOL(allocator);
    
    int blockNum = 0;
    
    yx_core_mempool_buf_head* buf_head;
    yx_core_listnode_ownerForEach(buf_head, &(context->buf_list), node)
    {
        blockNum++;
    }
    
    return blockNum;
}
yx_int yx_memsection_largeblocknum(yx_allocator allocator)
{
    yx_core_mempool_context* context = HANDLE2BUFMEMPOOL(allocator);
    
    int blockNum = 0;
    
    yx_core_mempool_large_block* largeBlock;
    yx_core_listnode_ownerForEach(largeBlock, &(context->largeblock_list), node)
    {
        blockNum++;
    }
    
    return blockNum;
}
yx_int yx_memsection_freelargeblocknum(yx_allocator allocator)
{
    yx_core_mempool_context* context = HANDLE2BUFMEMPOOL(allocator);
    
    int blockNum = 0;
    
    yx_core_mempool_large_block* largeBlock;
    yx_core_listnode_ownerForEach(largeBlock, &(context->empty_largeblock_list), node)
    {
        blockNum++;
    }
    
    return blockNum;
    
}

yx_size yx_memsection_remainSpace(yx_allocator allocator)
{
    yx_core_mempool_context* context = HANDLE2BUFMEMPOOL(allocator);
    
    yx_size remain_size = 0;
    
    yx_core_mempool_buf_head* buf_head = context->current;
    yx_core_listnode_ownerForEachFrom(buf_head, &(context->buf_list), node)
    {
        remain_size += (yx_size)(buf_head->end - buf_head->last);
    }
    
    return remain_size;
}
yx_size yx_memsection_waistSpace(yx_allocator allocator)
{
    yx_core_mempool_context* context = HANDLE2BUFMEMPOOL(allocator);
    
    yx_size waist_size = 0;
    
    yx_core_mempool_buf_head* buf_head;
    
    yx_core_listnode_ownerForEachTo(buf_head, &(context->buf_list), node, &(context->current->node))
    {
        waist_size += (yx_size)(buf_head->end - buf_head->last);
    }
    
    return waist_size;
}
yx_size yx_memsection_structureSpace(yx_allocator allocator)
{
    yx_size contextSize = sizeof(yx_core_mempool_context);
    yx_size blockSize = sizeof(yx_core_mempool_buf_head) * (yx_size)yx_memsection_blocknum(allocator);
    yx_size usedLargeBlockSize = sizeof(yx_core_mempool_large_block) * (yx_size)yx_memsection_largeblocknum(allocator);
    
    yx_size freeLargeBlockSize = sizeof(yx_core_mempool_large_block) * (yx_size)yx_memsection_freelargeblocknum(allocator);
    
    return (contextSize + blockSize + usedLargeBlockSize + freeLargeBlockSize);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
static yx_core_mempool_buf_head* _create_new_buf(yx_core_mempool_context* context)
{
    
    yx_size size = context->max_size;
    yx_core_mempool_buf_head* buf_head = yx_allocator_alloc(context->allocator, sizeof(yx_core_mempool_buf_head) + size);
    
    if(yx_unlikely( !buf_head )) {
        return NULL;
    }
    
    buf_head->last = (yx_ptr)(buf_head) + sizeof(yx_core_mempool_buf_head);
    buf_head->end = (yx_ptr)(buf_head->last) + size;
    buf_head->failed = 0;
    
    
    yx_core_listnode_insertTail(&(context->buf_list), &(buf_head->node));

    
    return buf_head;
}

static void* _alloc_from_base_pool(yx_core_mempool_context* context, yx_size size)
{
    void* mem = yx_allocator_alloc(context->allocator, size);
    if (yx_unlikely( !mem )) {
        return NULL;
    }
    
    yx_core_mempool_large_block* largeBlock;
    if (yx_core_listnode_isEmpty( &(context->empty_largeblock_list) ))
    {
        largeBlock = yx_memsection_alloc(context, sizeof(yx_core_mempool_large_block));
        largeBlock->mem = mem;
        yx_core_listnode_insertHead(&(context->largeblock_list), &(largeBlock->node));
    }
    else
    {
        largeBlock = yx_core_listnode_firstNodeOwner(&(context->empty_largeblock_list), yx_core_mempool_large_block, node);
        yx_core_listnode_del(&(largeBlock->node));
        largeBlock->mem = mem;
        yx_core_listnode_insertHead(&(context->largeblock_list), &(largeBlock->node));
    }
    
    
    
    return mem;
}

static yx_size _maxsize_of_buf()
{
    /*
     * for basic pool
     * NGX_MAX_ALLOC_FROM_POOL should be (ngx_pagesize - YX_ALIGNMENT).
     * On Windows NT it decreases a number of locked pages in a kernel.
     *
     */
    return (yx_size)yx_os_system_pagesize() - YX_ALIGNMENT;
}


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
                allocator = ((yx_core_mempool_context*)allocator)->allocator;
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





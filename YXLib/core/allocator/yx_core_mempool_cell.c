//
//  yx_core_mempool_cell.c
//  YXLib
//
//  Created by LiuYuxi on 15/6/29.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_mempool_cell.h"

#include "../container/yx_core_stacknode.h"
#include "../debug/yx_core_assert.h"
#include "../rttidef/yx_core_rttidef.h"


typedef struct _yx_core_mempool_cell_context_{
    YX_ALLOCATOR_STRUCT_DECLARE;
    
    yx_allocator parentpool;
    struct yx_core_stackhead cell_stack;
    
    yx_size cellSize;
    
}yx_core_mempool_cell_context;


////////////////////////////////////////////////////////////////////////////////////////////////////
yx_forceinline
yx_ptr _yx_cellMempool_alloc(yx_core_mempool_cell_context* context)
{
    
    if ( yx_core_stacknode_isEmpty(&(context->cell_stack)) )
    {
        return yx_allocator_alloc(context->parentpool, context->cellSize);
    }
    else
    {
        yx_ptr ptr = yx_core_stacknode_top(&(context->cell_stack));
        yx_core_stacknode_pop(&(context->cell_stack));
        
        return ptr;
    }
}
        
yx_forceinline
void _yx_cellMempool_free(yx_core_mempool_cell_context* context, yx_ptr ptr)
{
    yx_core_stacknode_push((yx_core_slistnode_ref)ptr, &(context->cell_stack));
}
        
extern yx_ptr _yx_cellMempool_std_alloc(yx_allocator allocator, yx_size size);
extern void* _yx_cellMempoo_std_memalign(yx_allocator allocator, yx_size alignment, yx_size size);
extern void _yx_cellMempoo_std_free(yx_allocator allocator, yx_ptr address);


////////////////////////////////////////////////////////////////////////////////////////////////////


yx_allocator yx_cellMempool_create(yx_allocator parentpool, yx_size cellSize)
{
    if (NULL == parentpool) {
        parentpool = yx_get_allocator();
    }
    
    YX_ASSERT( cellSize >= sizeof(struct yx_core_slistnode) );
    
    
    /*create the context*/
    yx_core_mempool_cell_context* context = yx_allocator_alloc(parentpool, sizeof(yx_core_mempool_cell_context));
    if (yx_unlikely( !context )) {
        return NULL;
    }
    
    context->parentpool = parentpool;
    context->cellSize = cellSize;
    yx_core_stacknode_init(&(context->cell_stack));
    
    
    yx_allocator_setup(yx_rtti_allocator_cell,
                       context,
                       _yx_cellMempool_std_alloc,
                       _yx_cellMempoo_std_memalign,
                       _yx_cellMempoo_std_free);
    
    return (yx_allocator)context;

}
void yx_cellMempool_destroy(yx_allocator* allocator_ref)
{
    yx_core_mempool_cell_context* context = *allocator_ref;
    *allocator_ref = NULL;
    
    
    while ( yx_false == yx_core_stacknode_isEmpty( &(context->cell_stack) ) )
    {
        yx_ptr ptr = yx_core_stacknode_top( &(context->cell_stack) );
        yx_core_stacknode_pop(&(context->cell_stack));
        yx_allocator_free(context->parentpool, ptr);
    }
           
    yx_allocator_free(context->parentpool, context);
    
}

yx_ptr yx_cellMempool_alloc(yx_allocator allocator)
{
    return _yx_cellMempool_alloc((yx_core_mempool_cell_context*)allocator);
}
           
void yx_cellMempool_free(yx_allocator allocator, yx_ptr ptr)
{
    _yx_cellMempool_free((yx_core_mempool_cell_context*)allocator, ptr);
}

/*get parent allocator*/
yx_allocator yx_cellMempool_parentPool(yx_allocator allocator)
{
    return ((yx_core_mempool_cell_context*)allocator)->parentpool;
}

           
yx_ptr _yx_cellMempool_std_alloc(yx_allocator allocator, yx_size size)
{
    yx_core_mempool_cell_context* context = (yx_core_mempool_cell_context*)allocator;
    YX_ASSERT(context->cellSize == size);
            
    return _yx_cellMempool_alloc(context);
}
           
void* _yx_cellMempoo_std_memalign(yx_allocator allocator, yx_size alignment, yx_size size)
{
    yx_unused(allocator);
    yx_unused(alignment);
    yx_unused(size);
    
    /*why you get here ?!!*/
    YX_ASSERT(0);
    
    return NULL;
}
       
void _yx_cellMempoo_std_free(yx_allocator allocator, yx_ptr address)
{
    yx_core_mempool_cell_context* context = (yx_core_mempool_cell_context*)allocator;
    
    _yx_cellMempool_free(context, address);
}








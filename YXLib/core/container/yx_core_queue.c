//
//  yx_core_queue.c
//  YXLib
//
//  Created by LiuYuxi on 15/7/1.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_queue.h"
#include "../rttidef/yx_core_rttidef.h"
#include "../allocator/yx_core_mempool_cell.h"
#include "../debug/yx_core_assert.h"


/*create and destroy*/
yx_core_queue_ptr yx_core_queue_create(yx_allocator allocator)
{
    if (NULL == allocator)
        allocator = yx_get_allocator();
    
    yx_core_queue_ptr queue = (yx_core_queue_ptr)yx_allocator_alloc(allocator, sizeof(struct yx_core_queue));
    
    yx_core_queue_init(allocator, queue);
    
    return queue;
}

void yx_core_queue_init(yx_allocator allocator, yx_core_queue_ptr queue)
{
    YX_ASSERT(NULL != allocator);
    YX_ASSERT(NULL != queue);
    
    /*setup the rtti info*/
    queue->rtti = yx_core_rtti_queue;
    
    /*setup the allocator*/
    queue->allocator = allocator;
    
    /*setup the node allocator*/
    yx_os_rtti_if(allocator, yx_rtti_allocator_buf) {
        /*因为是bufpool, 所以这个cellMempool不会被释放*/
        queue->node_allocator = yx_cellMempool_create(allocator, sizeof(struct yx_core_queuenode_wrapper));
    }
    else {
        queue->node_allocator = allocator;
    }
    
    /*init the queue*/
    yx_core_queuenode_Init(&(queue->head));
    
    
    /*init count*/
    queue->count = 0;
    
    
    /*init the del_callback*/
    queue->del_callback = NULL;

}



void yx_core_queue_destroy(yx_core_queue_ptr* queue_ptr)
{
    yx_core_queue_ptr queue = *queue_ptr;
    *queue_ptr = NULL;
    
    yx_core_queue_recycle(queue);
    
    yx_allocator_free(queue->allocator, queue);
}

void yx_core_queue_recycle(yx_core_queue_ptr queue)
{
    yx_os_rtti_notif(queue->allocator, yx_rtti_allocator_buf)
    {
        const yx_ulong count = queue->count;
        
        for (yx_ulong i=0; i < count; i++)
        {
            struct yx_core_queuenode_wrapper* node = yx_core_queuenode_firstNodeOwner(&(queue->head), struct yx_core_queuenode_wrapper , node);
            
            if (NULL != queue->del_callback)
                queue->del_callback(node->data);
            
            yx_core_queuenode_pop(&(queue->head));
            
            yx_allocator_free(queue->node_allocator, node);
        }
        
    }
    else if(NULL != queue->del_callback)
    {
        struct yx_core_queuenode_wrapper* cursor;
        yx_core_slistnode_ownerForEach(cursor, &(queue->head.node), node)
        {
            queue->del_callback(cursor->data);
        }
    }
}



/*state*/
//yx_bool yx_core_queue_isEmpty(yx_core_queue_ptr queue)
//{
//    return yx_core_queuenode_isEmpty(&(queue->head));
//}




/*get*/
//yx_value yx_core_queue_first(yx_core_queue_ptr queue)
//{
//    return yx_core_queuenode_firstNodeOwner(&(queue->head), struct yx_core_queuenode_wrapper, node)->data;
//}
//yx_value yx_core_queue_last(yx_core_queue_ptr queue)
//{
//    return yx_core_queuenode_lastNodeOwner(&(queue->head), struct yx_core_queuenode_wrapper, node)->data;
//}

/*push*/
void yx_core_queue_push(yx_core_queue_ptr queue, yx_value data)
{
    
    yx_core_queuenode_wrapper_ref new_node;
    
    new_node = yx_allocator_alloc(queue->node_allocator, sizeof(struct yx_core_queuenode_wrapper));
    if (yx_unlikely( !new_node )){
        /*
         //:~ LOG
         */
        return;
    }
    
    new_node->data = data;
    yx_core_queuenode_push(&(new_node->node), &(queue->head));
    queue->count++;
}

/*pop*/
void yx_core_queue_pop(yx_core_queue_ptr queue)
{

    yx_core_queuenode_wrapper_ref node = yx_core_queuenode_firstNodeOwner(&(queue->head), struct yx_core_queuenode_wrapper, node);
    
    yx_core_queuenode_pop(&(queue->head));
    queue->count--;
    
    if (NULL != queue->del_callback)
        queue->del_callback(node->data);
    
    yx_allocator_free(queue->node_allocator, node);
}






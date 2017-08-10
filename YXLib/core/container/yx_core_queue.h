//
//  yx_core_queue.h
//  YXLib
//
//  Created by LiuYuxi on 15/7/1.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_QUEUE_H_
#define _YX_CORE_QUEUE_H_ 

#include "yx_core_queuenode.h"

struct yx_core_queue{
    yx_rtti rtti;

    yx_allocator allocator;
    yx_allocator node_allocator;
    
    struct yx_core_queuehead head;
    yx_ulong count;
    
    void (*del_callback)(yx_value data);
};

typedef struct yx_core_queue* yx_core_queue_ptr;


struct yx_core_queuenode_wrapper{
    struct yx_core_slistnode node;
    yx_value data;
};

typedef struct yx_core_queuenode_wrapper* yx_core_queuenode_wrapper_ref;


/*create and destroy*/
yx_core_queue_ptr yx_core_queue_create(yx_allocator allocator);
void yx_core_queue_init(yx_allocator allocator, yx_core_queue_ptr queue); //init中allocator不能为NULL

void yx_core_queue_destroy(yx_core_queue_ptr* queue_ptr);
void yx_core_queue_recycle(yx_core_queue_ptr queue);



/*state*/
//yx_bool yx_core_queue_isEmpty(yx_core_queue_ptr queue);
#define yx_core_queue_isEmpty(queue) yx_core_queuenode_isEmpty(&((queue)->head))
#define yx_core_queue_count(queue) ((queue)->count)


/*get*/
//yx_value yx_core_queue_first(yx_core_queue_ptr queue);
#define yx_core_queue_first(queue)  \
    (yx_core_queuenode_firstNodeOwner(&((queue)->head), struct yx_core_queuenode_wrapper, node)->data)

//yx_value yx_core_queue_last(yx_core_queue_ptr queue);
#define yx_core_queue_last(queue) \
    (yx_core_queuenode_lastNodeOwner(&((queue)->head), struct yx_core_queuenode_wrapper, node)->data)


/*push*/
void yx_core_queue_push(yx_core_queue_ptr queue, yx_value data);

/*pop*/
void yx_core_queue_pop(yx_core_queue_ptr queue);





#endif /* defined(_YX_CORE_QUEUE_H_) */







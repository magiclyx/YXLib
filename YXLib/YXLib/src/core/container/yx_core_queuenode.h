//
//  yx_core_queuenode.h
//  YXLib
//
//  Created by LiuYuxi on 15/6/30.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_QUEUENODE_H_
#define _YX_CORE_QUEUENODE_H_

#include "yx_core_slistnode.h"

/*
 implemented an queue with slistnode
 */

struct yx_core_queuehead{
    struct yx_core_slistnode node;
    yx_core_slistnode_ref pTail;
};

typedef struct yx_core_queuehead* yx_core_queuehead_ref;



/*init*/
static inline void yx_core_queuenode_Init(yx_core_queuehead_ref head)
{
    head->node.next = &(head->node);
    head->pTail = &((head)->node);
}


/*state*/
#define yx_core_queuenode_isEmpty(head) \
    ( (head)->pTail == &((head)->node) )


/*get*/
#define yx_core_queuenode_firstNode(head) \
    ((head)->node.next)

#define yx_core_queuenode_firstNodeOwner(head, struct_type, node_name) \
    YX_CONTAINER_OF(yx_core_queuenode_firstNode(head), struct_type, node_name)

#define yx_core_queuenode_lastNode(head) \
    ((head)->pTail)

#define yx_core_queuenode_lastNodeOwner(head, struct_type, node_name) \
    YX_CONTAINER_OF(yx_core_queuenode_lastNode(head), struct_type, node_name)


/*push*/
#define yx_core_queuenode_push(newNode, head) \
    (newNode)->next = &((head)->node); \
    (head)->pTail->next = (newNode); \
    (head)->pTail = (newNode)


/*pop*/
static inline yx_core_slistnode_ref yx_core_queuenode_pop(yx_core_queuehead_ref head)
{
    yx_core_slistnode_ref rt_node = yx_core_queuenode_firstNode(head);
    head->node.next = rt_node->next;
    
    if (head->pTail == rt_node)
        head->pTail = &(head->node);
    
    return rt_node;
    
}





#endif /* defined(_YX_CORE_QUEUENODE_H_) */

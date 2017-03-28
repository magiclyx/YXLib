//
//  yx_core_stacknode.h
//  YXLib
//
//  Created by LiuYuxi on 15/6/30.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_STACKNODE_H_
#define _YX_CORE_STACKNODE_H_

#include "yx_core_slistnode.h"

struct yx_core_stackhead{
    struct yx_core_slistnode node;
};

typedef struct yx_core_stackhead* yx_core_stackhead_ref;


/*init*/
static inline void yx_core_stacknode_init(yx_core_stackhead_ref head)
{
    yx_core_slistnode_Init( &((head)->node) );
}

/*state*/
#define yx_core_stacknode_isEmpty(head) \
    yx_core_slistnode_isEmpty( &((head)->node) )

/*top*/
#define yx_core_stacknode_top(head) \
    yx_core_slistnode_firstNode( &((head)->node) )

/*push*/
#define yx_core_stacknode_push(newNode, head) \
    __yx_core_slistnode_add( (newNode), &((head)->node) )

/*pop*/
#define yx_core_stacknode_pop(head) \
    __yx_core_slistnode_del( &((head)->node) )

#endif /* defined(__YXLib__yx_core_stacknode__) */





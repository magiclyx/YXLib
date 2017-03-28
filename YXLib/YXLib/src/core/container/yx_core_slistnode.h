//
//  yx_core_slistnode.h
//  YXLib
//
//  Created by LiuYuxi on 15/6/26.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_SLISTNODE_H_
#define _YX_CORE_SLISTNODE_H_

#include "../yx_core_global.h"

struct yx_core_slistnode
{
    struct yx_core_slistnode* next;
};

typedef struct yx_core_slistnode* yx_core_slistnode_ref;





/*init the slist head*/

#define YX_CORE_SLISTNODE_SENTINEL(name) {&(name)}

#define YX_CORE_SLISTNODE_SENTINELINIT(name) \
    struct yx_core_slistnode name = YX_CORE_SLISTNODE_SENTINEL(name);

static inline void yx_core_slistnode_Init(yx_core_slistnode_ref sentinel_node)
{
    sentinel_node->next = sentinel_node;
}




/*node iterator - next node*/
#define yx_core_slistnode_nextNode(node) \
    ((node)->next)

#define yx_core_slistnode_nextNodeOwner(node, struct_type, node_name) \
    yx_core_slistnode_owner(yx_core_slistnode_nextNode(node), struct_type, node_name)


/*get the first node*/
#define yx_core_slistnode_firstNode(sentinel_node) \
    yx_core_slistnode_nextNode(sentinel_node)

#define yx_core_slistnode_firstNodeOwner(head, struct_type, node_name) \
    yx_core_slistnode_owner(yx_core_slistnode_firstNode(head), struct_type, node_name)



/*
 * Insert a new node after a specific node
 */

yx_forceinline
void __yx_core_slistnode_add(yx_core_slistnode_ref new_node,
                             yx_core_slistnode_ref prev_node)
{
    new_node->next = prev_node->next;
    prev_node->next = new_node;
}


/*insert the node in front of the specific node head*/
static inline void yx_core_slistnode_push(yx_core_slistnode_ref new_node,
                                          yx_core_slistnode_ref sentinel_node)
{
    __yx_core_slistnode_add(new_node, sentinel_node);
}



/*
 Delete a node after a specific node
 
 //the list node must not empty !!!!!!!
 */

yx_forceinline
void __yx_core_slistnode_del(yx_core_slistnode_ref prev_node)
{
    prev_node->next = prev_node->next->next;
}

static yx_inline void yx_core_slistnode_pop(yx_core_slistnode_ref sentinel_node)
{
    __yx_core_slistnode_del(sentinel_node);
}


/*
 slist_is_last - test wheter @slist is the last one
 */
static inline yx_bool yx_core_slistnode_isLast(const yx_core_slistnode_ref node,
                                               const yx_core_slistnode_ref sentinel_node)
{
    return (yx_core_slistnode_nextNode(node) == sentinel_node)? yx_true : yx_false;
}

static inline yx_bool yx_core_slistnode_isSentinel(const yx_core_slistnode_ref node,
                                                  const yx_core_slistnode_ref sentinel_node)
{
    return (node == sentinel_node)? yx_true : yx_false;
}



/**
 * tests whether a list is empty
 */
#define yx_core_slistnode_isEmpty(sentinel_node) \
    ( (yx_core_slistnode_nextNode(sentinel_node) == sentinel_node )? yx_true : yx_false )

/*
 return the node's owner's address
 node        the node
 struct_type  the structType
 node_name   the name of the node
 */
//#define yx_core_listnode_owner(sentinel_node, owner_type, node_name)                     \
//    (owner_type *) ((u_char *) sentinel_node - offsetof(owner_type, node_name))

#define yx_core_slistnode_owner(node, struct_type, node_name) ({                      \
    const typeof( ((struct_type *)0)->node_name ) *__mptr = (node);    \
    (struct_type *)( (char *)__mptr - offsetof(struct_type, node_name) );})

//#define yx_core_slistnode_safeOwner(node, struct_type, node_name) \
//    ( (NULL != node)? (yx_core_slistnode_owner(node, struct_type, node_name)) : NULL )


/**
 * slist_for_each	-	iterate over a list
 * @cursor:	the &struct yx_core_listnode to use as a loop cursor.
 * @sentinel_node:	the head for your list.
 */
#define yx_core_slistnode_forEach(cursor, sentinel_node) \
for (cursor = (sentinel_node)->next; cursor != (sentinel_node); cursor = cursor->next)

#define yx_core_slistnode_ownerForEach(cursor, sentinel_node, node_name) \
    for (cursor = yx_core_slistnode_owner((sentinel_node)->next, typeof(*cursor), node_name); \
        &(cursor->node_name) != (sentinel_node); \
        cursor = yx_core_slistnode_owner(cursor->node_name.next, typeof(*cursor), node_name))



#define yx_core_slistnode_forEachFrom(cursor, sentinel_node) \
    for (; cursor != (sentinel_node); cursor = cursor->next)

#define yx_core_slistnode_ownerForEachFrom(cursor, sentinel_node, node_name) \
    for (; \
        &(cursor->node_name) != (sentinel_node); \
        cursor = yx_core_slistnode_owner(cursor->node_name.next, typeof(*cursor), node_name))

#define yx_core_slistnode_forEachTo(cursor, sentinel_node, stop_node) \
    for (cursor = (sentinel_node)->next; cursor != (stop_node); cursor = cursor->next)

#define yx_core_slistnode_ownerForEachTo(cursor, sentinel_node, node_name, stop_node) \
    for (cursor = yx_core_slistnode_owner((sentinel_node)->next, typeof(*cursor), node_name); \
        &(cursor->node_name) != (stop_node); \
        cursor = yx_core_slistnode_owner(cursor->node_name.next, typeof(*cursor), node_name))



/*
 can not implement safe interate macro
 */






#endif /* defined(__YXLib__yx_core_slistnode__) */




















































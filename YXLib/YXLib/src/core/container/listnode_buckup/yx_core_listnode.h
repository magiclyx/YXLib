//
//  yx_core_list.h
//  YXLib
//
//  Created by LiuYuxi on 15/4/14.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_LISTNODE_H
#define _YX_CORE_LISTNODE_H

#include "../yx_core_global.h"

struct yx_core_listnode
{
    struct yx_core_listnode* prev;
    struct yx_core_listnode* next;
};

typedef struct yx_core_listnode* yx_core_listnode_ref;





/*init the list head*/

#define YX_CORE_LISTNODE_SENTINEL(name) { &(name), &(name) }

#define YX_CORE_LISTNODE_SENTINELINIT(name) \
            struct yx_core_listNode name = YX_CORE_LISTNODE_HEAD(name)


static inline void yx_core_listnode_sentinelInit(yx_core_listnode_ref sentinel_node)
{
    sentinel_node->next = sentinel_node;
    sentinel_node->prev = sentinel_node;
}



/*node iterator*/
#define yx_core_listnode_nextNode(node) \
        ((node)->next)

#define yx_core_listnode_prevNode(node) \
        ((node)->prev)


/*get the first node*/
static inline yx_core_listnode_ref yx_core_listnode_firstNode(yx_core_listnode_ref sentinel_node)
{
    return yx_core_listnode_nextNode(sentinel_node);
}


/*get the last node*/
static inline yx_core_listnode_ref yx_core_listnode_lastNode(yx_core_listnode_ref sentinel_node)
{
    return yx_core_listnode_prevNode(sentinel_node);
}



/*
 * Insert a new node between two known consecutive node.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __yx_core_listnode_add(yx_core_listnode_ref new_node,
                                          yx_core_listnode_ref prev_node,
                                          yx_core_listnode_ref next_node)
{
    next_node->prev = new_node;
    new_node->next = next_node;
    new_node->prev = prev_node;
    prev_node->next = new_node;
}


/*
 insert the node in front of the specific node head
 */
static inline void yx_core_listnode_insertHead(yx_core_listnode_ref sentinel_node,
                                               yx_core_listnode_ref new_node)
{
    __yx_core_listnode_add(new_node, sentinel_node, sentinel_node->next);
}


/**
 insert the node at the end of the list
 */
static inline void yx_core_listnode_insertTail(yx_core_listnode_ref sentinel_node,
                                               yx_core_listnode_ref new_node)
{
    __yx_core_listnode_add(new_node, sentinel_node->prev, sentinel_node);
}

/**
 insert a new node after a node
 */
#define yx_core_listnode_insertAfterNode(node, new_node) \
            yx_core_listnode_insertHead(node, new_node)

/**
 insert a new node in front of a node
 */
#define yx_core_listnode_insertBeforeNode(node, new_node) \
            yx_core_listnode_insertTail(node, new_node)


/*
 * Delete a list node by making the prev/next node
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next node already!
 */
static inline void __yx_core_listnode_del(yx_core_listnode_ref prev_node,
                                          yx_core_listnode_ref next_node)
{
    next_node->prev = prev_node;
    prev_node->next = next_node;
}


/**
 * delete a node
 */
static inline void yx_core_listnode_del(yx_core_listnode_ref node)
{
    __yx_core_listnode_del(node->prev, node->next);
    
#if YX_DEBUG
    node->prev = NULL;
    node->next = NULL;
#endif
}


/**
 * replace old node by new node
 */
static inline void yx_core_listnode_replace(yx_core_listnode_ref old_node,
                                            yx_core_listnode_ref new_node)
{
    new_node->next = old_node->next;
    new_node->next->prev = new_node;
    new_node->prev = old_node->prev;
    new_node->prev->next = new_node;
}


/**
 * delete a node from one list and add to another node's head
 */
static inline void yx_core_listnode_moveHead(yx_core_listnode_ref node,
                                             yx_core_listnode_ref new_sentinel_node)
{
    __yx_core_listnode_del(node->prev, node->next);
    yx_core_listnode_insertHead(new_sentinel_node, node);
}



/**
 * list_move_tail - delete a node from one list and add to the end of another list
 */
static inline void yx_core_listnode_moveTail(yx_core_listnode_ref node,
                                             yx_core_listnode_ref new_sentinel_node)
{
    __yx_core_listnode_del(node->prev, node->next);
    yx_core_listnode_insertTail(new_sentinel_node, node);
}


/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline yx_bool yx_core_listnode_isLast(const yx_core_listnode_ref node,
                                              const yx_core_listnode_ref sentinel_node)
{
    return (node->next == sentinel_node)? yx_true : yx_false;
}


/**
 * tests whether a list is empty
 */
static inline yx_bool yx_core_listnode_isEmpty(const yx_core_listnode_ref sentinel_node)
{
    return (sentinel_node->next == sentinel_node)? yx_true : yx_false;
}


/*
 return the node's owner's address
 node        the node
 struct_type  the structType
 node_name   the name of the node
 */
//#define yx_core_listnode_owner(sentinel_node, owner_type, node_name)                     \
//    (owner_type *) ((u_char *) sentinel_node - offsetof(owner_type, node_name))

#define yx_core_listnode_owner(node, struct_type, node_name) ({                      \
    const typeof( ((struct_type *)0)->node_name ) *__mptr = (node);    \
    (struct_type *)( (char *)__mptr - offsetof(struct_type, node_name) );})




/**
 * list_for_each	-	iterate over a list
 * @cursor:	the &struct yx_core_listnode to use as a loop cursor.
 * @sentinel_node:	the head for your list.
 */
#define yx_core_listnode_forEach(cursor, sentinel_node) \
for (cursor = (sentinel_node)->next; cursor != (sentinel_node); cursor = cursor->next)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @cursor:	the &struct yx_core_listnode to use as a loop cursor.
 * @n:		another &struct yx_core_listnode to use as temporary storage
 * @sentinel_node:	the sentinel_node for your list.
 */
#define list_core_listnode_forEachSafe(cursor, n, sentinel_node) \
for (cursor = (sentinel_node)->next, n = cursor->next; cursor != (sentinel_node); cursor = n, n = cursor->next)



#define yx_core_listnode_ownerForEach(cursor, sentinel_node) \
    for (cursor = (sentinel_node)->next; cursor != (sentinel_node); cursor = cursor->next)



/*get the middle node of a list*/
yx_core_listnode_ref yx_core_listnode_midNode(const yx_core_listnode_ref sentinel_node);


/*sort the list. (the stable insertion sort)*/
void yx_core_list_sort(yx_core_listnode_ref sentinel_node,
                       yx_int (*cmp)(const yx_core_listnode_ref node1, const yx_core_listnode_ref node2));


#endif /* defined(_YX_CORE_LISTNODE_H) */



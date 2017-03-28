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


/*
 考虑将链表的指针内置到宏里，
 
 因为实际使用的时候，头节点，和后面节点，往往不是同一类型，一次放弃了这种方式
 */


typedef yx_handle yx_core_listnode;


#define __YX_CORE_LISTNODE_PREV__(name) YX_CONCAT3(__yx_core_listnode_, name, _prev__)
#define __YX_CORE_LISTNODE_NEXT__(name) YX_CONCAT3(__yx_core_listnode_, name, _next__)


/*declare node*/
#define YX_CORE_LISTNODE_DECLARE(type, name) \
type* __YX_CORE_LISTNODE_PREV__(name); \
type* __YX_CORE_LISTNODE_NEXT__(name)


/*init the list head*/

#define yx_core_listnode_init(sentinel_node, name) \
        sentinel_node->__YX_CORE_LISTNODE_PREV__(name) = sentinel_node; \
        sentinel_node->__YX_CORE_LISTNODE_NEXT__(name) = sentinel_node;


/*node iterator*/
#define yx_core_listnode_nextNode(node, name) \
        ((node)->__YX_CORE_LISTNODE_NEXT__(name))

#define yx_core_listnode_prevNode(node, name) \
        ((node)->__YX_CORE_LISTNODE_PREV__(name))


/*get the first node*/
#define yx_core_listnode_firstNode(sentinel_node, name) \
        yx_core_listnode_nextNode(sentinel_node, name)

#define yx_core_listnode_lastNode(sentinel_node, name) \
        yx_core_listnode_prevNode(sentinel_node, name)




/*
 * Insert a new node between two known consecutive node.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
#define __yx_core_listnode_add(new_node, prev_node, next_node, name) \
        next_node->__YX_CORE_LISTNODE_PREV__(name) = new_node; \
        new_node->__YX_CORE_LISTNODE_NEXT__(name) = next_node; \
        new_node->__YX_CORE_LISTNODE_PREV__(name) = prev_node; \
        prev_node->__YX_CORE_LISTNODE_NEXT__(name) = new_node;



/*
 insert the node in front of the specific node head
 */
#define yx_core_listnode_insertHead(sentinel_node, new_node, name) \
        __yx_core_listnode_add(new_node, sentinel_node, sentinel_node->__YX_CORE_LISTNODE_NEXT__(name), name)


/**
 insert the node at the end of the list
 */
#define yx_core_listnode_insertTail(sentinel_node, new_node, name) \
        __yx_core_listnode_add(new_node, sentinel_node->__YX_CORE_LISTNODE_PREV__(name), sentinel_node, name)

/**
 insert a new node after a node
 */
#define yx_core_listnode_insertAfterNode(node, new_node, name) \
            yx_core_listnode_insertHead(node, new_node, name)

/**
 insert a new node in front of a node
 */
#define yx_core_listnode_insertBeforeNode(node, new_node, name) \
            yx_core_listnode_insertTail(node, new_node, name)


/*
 * Delete a list node by making the prev/next node
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next node already!
 */
#define __yx_core_listnode_del(prev_node, next_node, name) \
        next_node->__YX_CORE_LISTNODE_PREV__(name) = prev_node; \
        prev_node->__YX_CORE_LISTNODE_NEXT__(name) = next_node


/**
 * delete a node
 */

#if YX_DEBUG
    #define yx_core_listnode_del(node, name) \
            __yx_core_listnode_del(node->__YX_CORE_LISTNODE_PREV__(name), node->__YX_CORE_LISTNODE_NEXT__(name), name); \
            node->__YX_CORE_LISTNODE_PREV__(name) = NULL; \
            node->__YX_CORE_LISTNODE_NEXT__(name) = NULL
#else
    #define yx_core_listnode_del(node, name) \
            __yx_core_listnode_del(node->__YX_CORE_LISTNODE_PREV__(name), node->__YX_CORE_LISTNODE_NEXT__(name), name)
#endif


/**
 * replace old node by new node
 */
#define yx_core_listnode_replace(old_node, new_node, name) \
        new_node->__YX_CORE_LISTNODE_NEXT__(name) = old_node->__YX_CORE_LISTNODE_NEXT__(name); \
        new_node->__YX_CORE_LISTNODE_NEXT__(name)->__YX_CORE_LISTNODE_PREV__(name) = new_node; \
        new_node->__YX_CORE_LISTNODE_PREV__(name) = old_node->__YX_CORE_LISTNODE_PREV__(name); \
        new_node->__YX_CORE_LISTNODE_PREV__(name)->__YX_CORE_LISTNODE_NEXT__(name) = new_node


/**
 * delete a node from one list and add to another node's head
 */
#define yx_core_listnode_moveHead(node, new_sentinel_node, name) \
        __yx_core_listnode_del(node->__YX_CORE_LISTNODE_PREV__(name), node->__YX_CORE_LISTNODE_NEXT__(name), name); \
        yx_core_listnode_insertHead(new_sentinel_node, node, name)



/**
 * list_move_tail - delete a node from one list and add to the end of another list
 */
#define yx_core_listnode_moveTail(node, new_sentinel_node) \
        __yx_core_listnode_del(node->__YX_CORE_LISTNODE_PREV__(name), node->__YX_CORE_LISTNODE_NEXT__(name), name); \
        yx_core_listnode_insertTail(new_sentinel_node, node, name)


/**
 * list_is_last - tests whether @list is the last node in list
 * @list: the entry to test
 * @head: the head of the list
 */
#define yx_core_listnode_isLast(node, sentinel_node, name) \
    ((node->__YX_CORE_LISTNODE_NEXT__(name) == sentinel_node)? yx_true : yx_false)


/**
 * list_is_last - tests whether @list is the first node in list
 * @list: the entry to test
 * @head: the head of the list
 */
#define yx_core_listnode_isFirst(node, sentinel_node, name) \
    ((node->__YX_CORE_LISTNODE_PREV__(name) == sentinel_node)? yx_true : yx_false)

/**
 * tests whether a list is empty
 */
#define yx_core_listnode_isEmpty(sentinel_node, name) \
    ((sentinel_node->__YX_CORE_LISTNODE_NEXT__(name) == sentinel_node)? yx_true : yx_false)

//
///*
// return the node's owner's address
// node        the node
// struct_type  the structType
// node_name   the name of the node
// */
////#define yx_core_listnode_owner(sentinel_node, owner_type, node_name)                     \
////    (owner_type *) ((u_char *) sentinel_node - offsetof(owner_type, node_name))
//
//#define yx_core_listnode_owner(node, struct_type, node_name) ({                      \
//    const typeof( ((struct_type *)0)->node_name ) *__mptr = (node);    \
//    (struct_type *)( (char *)__mptr - offsetof(struct_type, node_name) );})
//
//


/**
 * list_for_each	-	iterate over a list
 * @cursor:	the &struct yx_core_listnode to use as a loop cursor.
 * @sentinel_node:	the head for your list.
 */
#define yx_core_listnode_forEach(cursor, sentinel_node, name) \
for (cursor = (sentinel_node)->__YX_CORE_LISTNODE_NEXT__(name); cursor != (sentinel_node); cursor = cursor->__YX_CORE_LISTNODE_NEXT__(name))

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @cursor:	the &struct yx_core_listnode to use as a loop cursor.
 * @n:		another &struct yx_core_listnode to use as temporary storage
 * @sentinel_node:	the sentinel_node for your list.
 */
#define list_core_listnode_forEachSafe(cursor, tmp_node, sentinel_node, name) \
for (cursor = (sentinel_node)->__YX_CORE_LISTNODE_NEXT__(name), tmp_node = cursor->__YX_CORE_LISTNODE_NEXT__(name); cursor != (sentinel_node); cursor = tmp_node, tmp_node = cursor->__YX_CORE_LISTNODE_NEXT__(name))





/*get the middle node of a list*/
//yx_core_listnode yx_core_listnode_midNode(const yx_core_listnode sentinel_node);


#define yx_core_listnode_midNode(sentinel_node, result_node_ref, name) \
do{ \
    typeof(sentinel_node) middle_node; \
    typeof(sentinel_node) last_node; \
    \
    middle_node = yx_core_listnode_firstNode(sentinel_node, name); \
    \
    if (middle_node == yx_core_listnode_lastNode(sentinel_node, name)) { \
        *result_node_ref = middle_node;\
    } \
    else{ \
        last_node = yx_core_listnode_firstNode(sentinel_node, name); \
        \
            for ( ;; ) { \
                middle_node = yx_core_listnode_nextNode(middle_node, name); \
                \
                last_node = yx_core_listnode_nextNode(last_node, name); \
                \
                if (last_node == yx_core_listnode_lastNode(sentinel_node, name)) { \
                    *result_node_ref = middle_node; \
                    break; \
                } \
                \
                last_node = yx_core_listnode_nextNode(last_node, name); \
                \
                if (last_node == yx_core_listnode_lastNode(sentinel_node, name)) { \
                    *result_node_ref = middle_node; \
                    break; \
                } \
            } \
    } \
    \
    \
}while(0)





/*sort the list. (the stable insertion sort)*/
void yx_core_list_sort(yx_core_listnode_ref sentinel_node,
                       yx_int (*cmp)(const yx_core_listnode_ref node1, const yx_core_listnode_ref node2));


#endif /* defined(_YX_CORE_LISTNODE_H) */



//
//  yx_core_rbtree.h
//  YXLib
//
//  Created by LiuYuxi on 15/6/26.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_RBTREE_H_
#define _YX_CORE_RBTREE_H_

#include "../yx_core_global.h"
#include "yx_core_rbnode.h"
#include "yx_core_listnode.h"
#include "../hash/yx_core_hash.h"

/*
 释放rbtree 或 removeAllData的时候，调用del_callback的顺序是不确定的
 
 -实际上,removeAllData的时候，总是广度优先便利
 释放rbtree的时候，如果mempool是bufpull.则增序遍历，否则是广度遍历
 
 */

//:~ TODO add const keyword

struct yx_core_rbtree
{
    yx_rtti rtti;
    
    struct yx_core_rbroot root;
    yx_allocator allocator;
    yx_allocator node_allocator;
    
    yx_ulong count;
    
    yx_compare_result (*compare_callback)(yx_value key1, yx_value key2); //use to compare two keys
    yx_core_hash (*hash_callback)(yx_value key); //use to get the key's hash value
    yx_value (*keycopy_callback)(yx_value key); //use to copy a key
    void (*del_callback)(yx_value data); //use to release data's buff
};

typedef struct yx_core_rbtree* yx_core_rbtree_ref;


struct yx_core_rbnode_wrapper
{
    struct yx_core_rbnode node;
    yx_core_hash key_hash;
    yx_value key;
    yx_value data;
};

typedef struct yx_core_rbnode_wrapper* yx_core_rbnode_wrapper_ref;

/**
 create & destroy
 */
yx_core_rbtree_ref yx_core_rbtree_create(yx_allocator allocator);
void yx_core_rbtree_init(yx_allocator allocator, yx_core_rbtree_ref tree);

void yx_core_rbtree_destroy(yx_core_rbtree_ref* tree_ptr);
void yx_core_rbtree_recycle(yx_core_rbtree_ref tree);


void yx_core_rbtree_set_callback_compare(yx_core_rbtree_ref rbtree, yx_compare_result (*compare_callback)(yx_value key1, yx_value key2));
void yx_core_rbtree_set_callback_hash(yx_core_rbtree_ref rbtree, yx_core_hash (*hash_callback)(yx_value key));

void yx_core_rbtree_set_callback_del(yx_core_rbtree_ref rbtree, void (*del_callback)(yx_value data));




/*
 set
 */
//overwrite when the key is same
void yx_core_rbtree_setDataForKey(yx_core_rbtree_ref rbtree, yx_value data, yx_value key);
//return yx_false when the key is same
yx_bool yx_core_rbtree_addDataForKey(yx_core_rbtree_ref rbtree, yx_value data, yx_value key);


/*
 get
 */
yx_value yx_core_rbtree_dataForKey(const yx_core_rbtree_ref rbtree, yx_value key, yx_value defalutData);
yx_bool yx_core_rbtree_isKeyExist(const yx_core_rbtree_ref rbtree, yx_value key);

/*
 del
 */
void yx_core_rbtree_removeDataForKey(yx_core_rbtree_ref rbtree, const yx_value key);
void yx_core_rbtree_removeAllData(yx_core_rbtree_ref rbtree);


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 walker
 */

//Breadth-first traversal
void yx_core_rbtree_breadthFirstTraversal(const yx_core_rbtree_ref rbtree, yx_value bindingData, void (*traversal_callback)(yx_value key, yx_value data, yx_value bindingData));

//------------------------------------------------------------------------------------

//inorder traversal -- ascending order
void yx_core_rbtree_inorderTraversal(const yx_core_rbtree_ref rbtree, yx_value bindingData, void (*traversal_callback)(yx_value key, yx_value data, yx_value bindingData));

//inorder traversal -- descending order
void yx_core_rbtree_inorderTraversal_reverse(const yx_core_rbtree_ref rbtree, yx_value bindingData, void (*traversal_callback)(yx_value key, yx_value data, yx_value bindingData));

//------------------------------------------------------------------------------------

//postorder traversal
void yx_core_rbtree_postorderTraversal(const yx_core_rbtree_ref rbtree, yx_value bindingData, void (*traversal_callback)(yx_value key, yx_value data, yx_value bindingData));


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "yx_core_queue.h"


//Breadth-first traversal

typedef struct yx_core_rbtree_breadth_cursor{
    yx_allocator bufAllocator;
    struct yx_core_queue queue;
    yx_core_rbnode_wrapper_ref node;
}yx_core_rbtree_breadth_cursor;

void yx_core_rbtree_traversal_cursor_init(
                                          const yx_core_rbtree_ref rbtree,
                                          yx_core_rbtree_breadth_cursor* cursor
                                          );

void yx_core_rbtree_traversal_cursor_recycle(yx_core_rbtree_breadth_cursor* cursor);

void yx_core_rbtree_traversal_cursor_next(yx_core_rbtree_breadth_cursor* cursor);


#define yx_core_rbtree_traversal_cursor_isEmpty(cursor) (NULL == (cursor)->node)


//------------------------------------------------------------------------------------
//inorder traversal

//当没有下一个的时候，返回NULL
yx_core_rbnode_wrapper_ref yx_core_rbtree_inorder_firstNode(const yx_core_rbtree_ref rbtree);
yx_core_rbnode_wrapper_ref yx_core_rbtree_inorder_lastNode(const yx_core_rbtree_ref rbtree);
yx_core_rbnode_wrapper_ref yx_core_rbtree_inorder_nextNode(yx_core_rbnode_wrapper_ref node_wraper);
yx_core_rbnode_wrapper_ref yx_core_rbtree_inorder_prevNode(yx_core_rbnode_wrapper_ref node_wraper);

//------------------------------------------------------------------------------------
//postorder traversal

//当没有下一个的时候，返回NULL
yx_core_rbnode_wrapper_ref yx_core_rbtree_postorder_firstNode(const yx_core_rbtree_ref rbtree);
yx_core_rbnode_wrapper_ref yx_core_rbtree_postorder_nextNode(yx_core_rbnode_wrapper_ref node_wraper);






#define yx_core_rbtree_forEach(rbtree, node_wrapper) \
    for (struct yx_core_rbnode* ___node = yx_core_rbnode_firstNode(rbtree); \
    ( ___node && (node_wrapper = yx_core_rbnode_owner(___node, struct yx_core_rbnode_wrapper, node)) );\
    ___node = yx_core_rbnode_nextNode(___node))


#endif /* defined(_YX_CORE_RBTREE_H_) */































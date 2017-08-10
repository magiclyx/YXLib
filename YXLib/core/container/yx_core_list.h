//
//  yx_core_list.h
//  YXLib
//
//  Created by LiuYuxi on 15/4/15.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_LIST_H_
#define _YX_CORE_LIST_H_

#include "../yx_core_global.h"
#include "yx_core_listnode.h"


typedef struct yx_core_list
{
    yx_rtti rtti;
    
    yx_allocator allocator;
    yx_allocator node_allocator;
    
    struct yx_core_listnode list;
    yx_ulong count;
    
    void (*del_callback)(yx_value data);
}yx_core_list;

typedef struct yx_core_list* yx_core_list_ref;


typedef struct yx_core_listnode_wrapper
{
    struct yx_core_listnode node;
    yx_value data;
}yx_core_listnode_wrapper;


/**
 create & destroy
 */
yx_core_list_ref yx_core_list_create(yx_allocator allocator);
void yx_core_list_init(yx_allocator allocator, yx_core_list_ref list);

void yx_core_list_destroy(yx_core_list_ref* list_ptr);
void yx_core_list_recycle(yx_core_list_ref list);

/**
 set call back function
 */
void yx_core_list_set_delCallback(yx_core_list_ref list, void (*del_callback)(yx_value data));

/**
 insert
 */
void yx_core_list_insertHead(yx_core_list_ref list, yx_value data);
void yx_core_list_insertTail(yx_core_list_ref list, yx_value data);

void yx_core_list_insertHeadFromList(yx_core_list_ref list, yx_core_list_ref another_list);
void yx_core_list_insertTailFromList(yx_core_list_ref list, yx_core_list_ref another_list);


/**
 state
 */
#define yx_core_list_isEmpty(list)  yx_core_listnode_isEmpty(&(list->list))
#define yx_core_list_count(list) ((list)->count)


/**
 get
 */
yx_value yx_core_list_firstObject(yx_core_list_ref list, yx_value default_value);
yx_value yx_core_list_lastObject(yx_core_list_ref list, yx_value default_value);

/**
 delete
 */
//Please always use this function to delete node in yx_core_list.
void yx_core_list_deleteNode(yx_core_list_ref list, struct yx_core_listnode_wrapper* node);
void yx_core_list_removeAll(yx_core_list_ref list);
void yx_core_list_removeFirstObject(yx_core_list_ref list);
void yx_core_list_removeLastObject(yx_core_list_ref list);







/**
 loop
 */
#define yx_core_list_foreEach(node_wrapper_ref, list_ref) yx_core_listnode_ownerForEach(node_wrapper_ref, &(list_ref->list), node)

#define yx_core_list_forEachSafe(node_wrapper_ref, n, list_ref) yx_core_listnode_ownerForEachSafe(node_wrapper_ref, n, &(list_ref->list), node)

#endif /* defined(_YX_CORE_LIST_H_) */






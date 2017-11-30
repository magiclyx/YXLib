//
//  yx_core_list.c
//  YXLib
//
//  Created by LiuYuxi on 15/4/15.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_list.h"

#include "../rttidef/yx_core_rttidef.h"
#include "../debug/yx_core_assert.h"
#include "../allocator/yx_core_mempool_cell.h"



yx_core_list_ref yx_core_list_create(yx_allocator allocator)
{
    if (NULL == allocator)
        allocator = yx_get_allocator();
    
    
    yx_core_list_ref list = (yx_core_list_ref)yx_allocator_alloc(allocator, sizeof(struct yx_core_list));
    
    
    yx_core_list_init(allocator, list);
    
    
    return list;
}

void yx_core_list_init(yx_allocator allocator, yx_core_list_ref list)
{
    /*setup the rtti info*/
    list->rtti = yx_core_rtti_list;
    
    
    /*setup the allocator*/
    list->allocator = allocator;
    
    
    /*setup the node allocator*/
    yx_os_rtti_if(allocator, yx_rtti_allocator_buf) {
        /*因为是bufpool, 所以这个cellMempool不会被释放*/
        list->node_allocator = yx_cellMempool_create(allocator, sizeof(struct yx_core_listnode_wrapper));
    }
    else {
        list->node_allocator = allocator;
    }
    
    /*init the list*/
    yx_core_listnode_sentinelInit(&(list->list));
    
    /*init the count*/
    list->count = 0;
}


void yx_core_list_set_delCallback(yx_core_list_ref list, void (*del_callback)(yx_value data))
{
    list->del_callback = del_callback;
}

void yx_core_list_destroy(yx_core_list_ref* list_ptr)
{
    yx_core_list_ref list = *list_ptr;
    *list_ptr = NULL;
    
    
    yx_core_list_recycle(list);

    yx_allocator_free(list->allocator, list);
}

void yx_core_list_recycle(yx_core_list_ref list)
{
    yx_os_rtti_notif(list->allocator, yx_rtti_allocator_buf)
    {
        /*其他内存池，正常释放*/
        
        struct yx_core_listnode_wrapper* cursor;
        struct yx_core_listnode_wrapper* tmpNode;
        
        
        yx_core_listnode_ownerForEachSafe(cursor, tmpNode, &(list->list), node)
        {
            yx_core_listnode_del(&(cursor->node));
            
            if (NULL != list->del_callback) {
                list->del_callback(cursor->data);
            }
            
            yx_allocator_free(list->node_allocator, cursor);
        }
        
    }
    else if(NULL != list->del_callback)
    {
        /*buf memPool, 只有在有回调函数时，才回执行一次循环*/
        
        struct yx_core_listnode_wrapper* cursor;
        yx_core_listnode_ownerForEach(cursor, &(list->list), node)
        {
            list->del_callback(cursor->data);
        }
        
    }
}





void yx_core_list_insertHead(yx_core_list_ref list, yx_value data)
{
    struct yx_core_listnode_wrapper* new_node;
    
    
    new_node = yx_allocator_alloc(list->node_allocator, sizeof(struct yx_core_listnode_wrapper));
    if (yx_unlikely( !new_node )){
        /*
         //:~ LOG
         */
        return;
    }
    
    new_node->data = data;
    yx_core_listnode_insertHead(&(list->list), &(new_node->node));
    list->count++;
    
}


void yx_core_list_insertTail(yx_core_list_ref list, yx_value data)
{
    
    struct yx_core_listnode_wrapper* new_node;
    
    
    new_node = yx_allocator_alloc(list->node_allocator, sizeof(struct yx_core_listnode_wrapper));
    if (yx_unlikely( !new_node )) {
        //:~ LOG
        return;
    }
    
    new_node->data = data;
    
    yx_core_listnode_insertTail(&(list->list), &(new_node->node));
    list->count++;
    
}

void yx_core_list_insertHeadFromList(yx_core_list_ref list, yx_core_list_ref another_list)
{
    struct yx_core_listnode_wrapper* node;
    
    yx_core_listnode_ownerForEach(node, &(another_list->list), node)
    {
        yx_core_list_insertHead(list, node->data);
    }
}

void yx_core_list_insertTailFromList(yx_core_list_ref list, yx_core_list_ref another_list)
{
    
    struct yx_core_listnode_wrapper* node;
    
    yx_core_listnode_ownerForEach(node, &(another_list->list), node)
    {
        yx_core_list_insertTail(list, node->data);
    }

}


yx_value yx_core_list_firstObject(yx_core_list_ref list, yx_value default_value)
{
    yx_core_listnode_ref first_node = yx_core_listnode_firstNode(&(list->list));
    if (!yx_core_listnode_isSentinel(first_node, &(list->list)))
    {
        struct yx_core_listnode_wrapper* wrapper_node = yx_core_listnode_owner(first_node, struct yx_core_listnode_wrapper, node);
        
        return (wrapper_node->data);
    }
    
    return default_value;
}

yx_value yx_core_list_lastObject(yx_core_list_ref list, yx_value default_value)
{
    yx_core_listnode_ref last_node = yx_core_listnode_lastNode(&(list->list));
    if (!yx_core_listnode_isSentinel(last_node, &(list->list)))
    {
        struct yx_core_listnode_wrapper* wrapper_node = yx_core_listnode_owner(last_node, struct yx_core_listnode_wrapper, node);
        
        return (wrapper_node->data);
    }
    
    return default_value;

}



void yx_core_list_deleteNode(yx_core_list_ref list, struct yx_core_listnode_wrapper* node)
{
    
    yx_core_listnode_del(&(node->node));
    list->count--;
    
    if (list->del_callback) {
        list->del_callback(node->data);
    }
    yx_allocator_free(list->node_allocator, node);
    
}

void yx_core_list_removeAll(yx_core_list_ref list)
{
    
    struct yx_core_listnode_wrapper* cursor;
    struct yx_core_listnode_wrapper* tmpNode;
    
    
    yx_core_listnode_ownerForEachSafe(cursor, tmpNode, &(list->list), node)
    {
        yx_core_list_deleteNode(list, cursor);
    }
    
}



void yx_core_list_removeFirstObject(yx_core_list_ref list)
{
    yx_core_listnode_ref first_node = yx_core_listnode_firstNode(&(list->list));
    if (!yx_core_listnode_isSentinel(first_node, &(list->list)))
    {
        struct yx_core_listnode_wrapper* wrapper_node = yx_core_listnode_owner(first_node, struct yx_core_listnode_wrapper, node);
        
        yx_core_list_deleteNode(list, wrapper_node);
    }

}
void yx_core_list_removeLastObject(yx_core_list_ref list)
{
    yx_core_listnode_ref last_node = yx_core_listnode_lastNode(&(list->list));
    if (!yx_core_listnode_isSentinel(last_node, &(list->list)))
    {
        struct yx_core_listnode_wrapper* wrapper_node = yx_core_listnode_owner(last_node, struct yx_core_listnode_wrapper, node);
        
        yx_core_list_deleteNode(list, wrapper_node);
    }
}



//
//  yx_unittest_utility.c
//  YXLib
//
//  Created by Yuxi Liu on 9/8/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//


#include "yx_unittest_list.h"
#include "yx_os_headers.h"


typedef struct __yx_ut_node{
    struct __yx_ut_node* next;
    struct __yx_ut_node* prev;
    yx_handle data;
}_yx_ut_node_ctx, *_yx_ut_node_ctx_ref;

typedef struct __yx_ut_list{
    _yx_ut_node_ctx* head;
    _yx_ut_node_ctx* tail;
    yx_allocator allocator;
    
    yx_handle bindingData;
}_yx_ut_list_ctx, *_yx_ut_list_ctx_ref;



_yx_ut_list _yx_ut_list_create(yx_allocator allocator){
    
    if (NULL == allocator) {
        allocator = yx_get_allocator();
    }
    
    _yx_ut_list_ctx_ref context = (_yx_ut_list_ctx_ref)yx_allocator_alloc(allocator, sizeof(_yx_ut_list_ctx));
    
    context->head = NULL;
    context->tail = NULL;
    context->allocator = allocator;
    
    context->bindingData = NULL;
    
    return (_yx_ut_list)context;
}

void _yx_ut_list_destroy(_yx_ut_list* list_ptr, yx_ut_callback_destroy callback){
    _yx_ut_list_ctx_ref context = (_yx_ut_list_ctx_ref)(*list_ptr);
    *list_ptr = NULL;
    
    yx_allocator allocator = context->allocator;
    
    yx_index index = 0;
    while (NULL != context->head) {
        _yx_ut_node_ctx_ref tmpnode = context->head;
        context->head = tmpnode->next;
        
        if (NULL != callback)
            callback(context, tmpnode->data, 0);
        
        yx_allocator_free(allocator, tmpnode);
        index++;
    }
    
    yx_allocator_free(allocator, context);
}

void _yx_ut_list_insert_front(_yx_ut_list list, yx_handle handle){
    _yx_ut_list_ctx_ref context = (_yx_ut_list_ctx_ref)list;
    
    _yx_ut_node_ctx_ref node = (_yx_ut_node_ctx_ref)yx_allocator_alloc(context->allocator, sizeof(_yx_ut_node_ctx));
    
    node->data = handle;
    node->next = context->head;
    node->prev = NULL;
    context->head->prev = node;
    
    context->head = node;
    if (NULL == context->tail)
    {
        context->tail = node;
    }
    
}

void _yx_ut_list_insert_back(_yx_ut_list list, yx_handle handle){
    _yx_ut_list_ctx_ref context = (_yx_ut_list_ctx_ref)list;
    
    _yx_ut_node_ctx_ref node = (_yx_ut_node_ctx_ref)yx_allocator_alloc(context->allocator, sizeof(_yx_ut_node_ctx));
    
    node->data = handle;
    node->next = NULL;
    node->prev = context->tail;
    
    if (NULL != context->tail)
        context->tail->next = node;
    
    if (NULL == context->head)
        context->head = node;
    
    context->tail = node;
}

void _yx_ut_list_bindingData_set(_yx_ut_list list, yx_handle handle)
{
    _yx_ut_list_ctx_ref context = (_yx_ut_list_ctx_ref)list;
    context->bindingData = handle;
}

yx_handle _yx_ut_list_bindingData_get(_yx_ut_list list)
{
    _yx_ut_list_ctx_ref context = (_yx_ut_list_ctx_ref)list;
    
    return context->bindingData;
}



void _yx_ut_list_walk(_yx_ut_list list, yx_ut_callback_work callback){
    _yx_ut_list_ctx_ref context = (_yx_ut_list_ctx_ref)(list);
    
    YX_ASSERT(NULL != callback);
    
    _yx_ut_node_ctx_ref cursor = context->head;
    while (NULL != cursor) {
        
        if (yx_false == callback(list, cursor->data, 0))
            break;
        
        cursor = cursor->next;
    }
 
}

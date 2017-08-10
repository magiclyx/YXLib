//
//  yx_core_mempool_debug.c
//  YXLib
//
//  Created by Yuxi Liu on 1/5/15.
//  Copyright (c) 2015 Yuxi Liu. All rights reserved.
//

#include "yx_core_mempool_debug.h"
#include "yx_core_mem.h"

#include "../rttidef/yx_core_rttidef.h"
#include "../debug/yx_core_assert.h"


#define YXDEBUGPOOL_EXPAND_DEFAULT_NUM 512
#define YXDEBUGPOOL_EXPAND_MAX_NUM 4096
#define YXDEBUGPOOL_FRAME_MAX_LEVEL 128

typedef yx_uint32 __yx_debug_mempool_guard;

typedef struct __yx_debug_mempool_node_s_
{
    struct __yx_debug_mempool_node_s_* next;
    struct __yx_debug_mempool_node_s_* prev;
    
    void* mem; //the memory return to the user.
    void* allocated_mem; //the real memory allocated from the low level allocator
    
    __yx_debug_mempool_guard* header_guard;
    __yx_debug_mempool_guard* tail_guard;
    
    yx_char** frame_address;
    yx_int frame_level;
    
    
    yx_size alloced_size; //size alloced by the caller
    yx_size mem_size; //The actual allocation of the memory size
    
}__yx_debug_mempool_node, *__yx_debug_mempool_node_ref;


//struct __yx_debug_mempool_session_head_s_
//{
//    struct __yx_debug_mempool_node_s_* owner_node;
////    yx_uint32 head_guid;
//};

//struct __yx_debug_mempool_session_tail_s_
//{
//    yx_uint32 tail_guid;
//    YX_STRUCT_ALIGNMENT_INFILLING(sizeof(yx_uint32));
//};



struct __yx_debug_mempool_context_s_
{
    YX_ALLOCATOR_STRUCT_DECLARE;
    yx_allocator allocator;
    
    /*supporty stuff*/
    yx_uint32 next_expand_num;
    yx_uint32 last_error_code;
    struct __yx_debug_mempool_node_s_* used_list;
    struct __yx_debug_mempool_node_s_* free_list;
    
    /*optional*/
    yx_bool enable_assertion;
    
    /*statistics info*/
    yx_size mem_size; //current memory size alloced from the pool
    yx_size max_mem_size; //max memory size alloced from the pool
    yx_uint node_num; //total number of node in the pool
};

#pragma mark declare
////////////////////////////////////////////////////////////////////////////////////////////////////
/*declare*/
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _expand_free_list(struct __yx_debug_mempool_context_s_* context);
static yx_uint _expand_node_num(struct __yx_debug_mempool_context_s_* context);

static __yx_debug_mempool_node* _init_list(yx_allocator allocator);
static void _destroy_list(yx_allocator allocator, __yx_debug_mempool_node_ref* list_pt);
static void _insert_node_to_list(__yx_debug_mempool_node* list, __yx_debug_mempool_node* node);
static void _delete_node_from_list(__yx_debug_mempool_node* list, __yx_debug_mempool_node* node);
#define _is_list_empty(list) ( (NULL != list->next)? yx_false : yx_true )

static yx_bool _verify_the_mode(struct __yx_debug_mempool_context_s_* context, __yx_debug_mempool_node* node, yx_ptr address);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark basic operation
yx_allocator yx_debugMempool_create(yx_allocator allocator, yx_uint node_num)
{
    struct __yx_debug_mempool_context_s_* context = NULL;
    
    if (NULL == allocator)
        allocator = yx_get_allocator();
    
    context = yx_allocator_alloc(allocator, sizeof(struct __yx_debug_mempool_context_s_));
    if (NULL == context)
        goto errout;
    
    
    /*expand node num*/
    context->next_expand_num = node_num > 0? node_num : YXDEBUGPOOL_EXPAND_DEFAULT_NUM;
    
    /*last error code*/
    context->last_error_code = yx_debugMempool_eCode_success;
    
    
    /*init the list*/
    context->used_list = _init_list(allocator);
    context->free_list = _init_list(allocator);
    
    
    /* statistics info */
    
    //mem size
    context->mem_size = 0;
    context->max_mem_size = 0;
    context->node_num = 0;
    
    
    /*optional*/
    
    //enable assertion
    context->enable_assertion = yx_true;
    
    
    /*setup the allocator*/
    context->allocator = allocator;
    yx_allocator_setup(yx_rtti_allocator_debug, context, yx_debugMempool_alloc, yx_debugMempool_memalign, yx_debugMempool_free);
    
    
    
    
    
    
    /*add free node into empty_list*/
    _expand_free_list(context);
    
    
    return (yx_allocator)context;
    
errout:
    
    if (NULL != context) {
        if (NULL != context->used_list) {
            _destroy_list(allocator, &(context->used_list));
        }
        
        
        if (NULL != context->free_list) {
            _destroy_list(allocator, &(context->free_list));
        }
        
        yx_allocator_free(allocator, context);
    }
    
    
    return NULL;
}

void yx_debugMempool_destroy(yx_allocator* allocator_ptr)
{
    YX_ASSERT(NULL != allocator_ptr);
    YX_ASSERT(NULL != *allocator_ptr);
    
    struct __yx_debug_mempool_context_s_* context = (struct __yx_debug_mempool_context_s_*)(*allocator_ptr);
    *allocator_ptr = NULL;
    
    yx_allocator allocator = context->allocator;
    
    YX_ASSERT(NULL != context->used_list);
    YX_ASSERT(NULL != context->free_list);
    
    
    context->last_error_code = yx_debugMempool_eCode_success;
    if (0 != context->mem_size) {
        
        /*memory leak !*/
        context->last_error_code = yx_debugMemPool_eCode_leak;
        
        if (yx_true == context->enable_assertion) {
            yx_debugMempool_printAllocatorStack((yx_allocator)context);
            
            YX_ASSERT(0);
        }
        
        /*free the leak memory*/
        struct __yx_debug_mempool_node_s_* node = context->used_list->next;
        while (NULL != node)
        {
            if (NULL != node->allocated_mem) {
                yx_allocator_free(allocator, node->allocated_mem);
            }
            
            node = node->next;
        }
        
        
    }
    
    
    
    
    
    _destroy_list(allocator, &(context->used_list));
    _destroy_list(allocator, &(context->free_list));
    
    yx_allocator_free(allocator, context);
}


void* yx_debugMempool_alloc(yx_allocator allocator, yx_size size)
{
    return yx_debugMempool_memalign(allocator, YX_ALIGNMENT, size);
}

void* yx_debugMempool_memalign(yx_allocator allocator, yx_size alignment, yx_size size)
{
    YX_ASSERT(NULL != allocator);
    struct __yx_debug_mempool_context_s_* context = (struct __yx_debug_mempool_context_s_*)(allocator);
    
    
    /*expand the free list if need*/
    if (yx_true == _is_list_empty(context->free_list)) {
        _expand_free_list(context);
    }
    
    /*fetch a empty node from the free_list*/
    __yx_debug_mempool_node* node = context->free_list->next;
    YX_ASSERT(NULL != node);
    
    /*delete the node from the free_list*/
    _delete_node_from_list(context->free_list, node);
    
    
    // we need to allocate enough storage for the requested bytes, some
    // book-keeping (to store the location returned by malloc) and some extra
    // padding to allow us to find an aligned byte. im not entirely sure if
    // 2 * alignment is enough here, its just a guess.
    const yx_size totalSize = sizeof(struct __yx_debug_mempool_node_s_*) + sizeof(__yx_debug_mempool_guard)*2 + 2*alignment + size;
    
    void* mem_start = yx_allocator_alloc(context->allocator, totalSize);
    if (NULL == mem_start)
        return NULL;
    
    // dedicate enough space to the book-keeping.
    void* mem = mem_start + sizeof(struct __yx_debug_mempool_node_s_*) + sizeof(__yx_debug_mempool_guard);
    
    
    // find a memory location with correct alignment. the alignment minus
    // the remainder of this mod operation is how many bytes forward we need
    // to move to find an aligned byte.
    //const size_t alignment_offset_value = alignment - (((size_t)mem) % alignment);
    
    
    // set data to the aligned memory.
    //mem += alignment_offset_value;
    mem = YX_POINT_ALIGNMENT(mem);
    
    
    /*setup the mem*/
    node->allocated_mem = mem_start;
    node->mem = mem;
    
    /*setup the header guard*/
//    mem_offset += sizeof(struct __yx_debug_mempool_node_s_*);
    __yx_debug_mempool_guard* head_guard = mem - sizeof(__yx_debug_mempool_guard);
    *head_guard = YX_MEMORY_MAGICNUM_HEAP_GUARD;
    node->header_guard = head_guard;
    
    
    /*setup the owner node*/
    struct __yx_debug_mempool_node_s_** owner_node_ptr = (yx_ptr)head_guard - sizeof(struct __yx_debug_mempool_node_s_*);
    *owner_node_ptr = node;
    
    
    /*setup the tail guard*/
    __yx_debug_mempool_guard* tail_guard = mem + size;
    *tail_guard = YX_MEMORY_MAGICNUM_HEAP_GUARD;
    node->tail_guard = tail_guard;
    
    
    /*insert the node into the used_list*/
    _insert_node_to_list(context->used_list, node);
    
    
    /*fill the frame info*/
    void* callstack[YXDEBUGPOOL_FRAME_MAX_LEVEL];
    node->frame_level = backtrace(callstack, YXDEBUGPOOL_FRAME_MAX_LEVEL);
    node->frame_address = backtrace_symbols(callstack, node->frame_level);
    
    
    /*set the statistic info*/
    context->mem_size += size;
    if (context->max_mem_size < context->mem_size) {
        context->max_mem_size = context->mem_size;
    }
    
    node->alloced_size = size;
    node->mem_size = totalSize;
    
    return mem;
}


void yx_debugMempool_free(yx_allocator allocator, yx_ptr address)
{
    if (NULL == address)
        return;
    
    /*context*/
    YX_ASSERT(NULL != allocator);
    struct __yx_debug_mempool_context_s_* context = (struct __yx_debug_mempool_context_s_*)(allocator);
    
    /*find the start of the memory*/
    void* mem_offset = address - sizeof(__yx_debug_mempool_guard) - sizeof(struct __yx_debug_mempool_node_s_*);
    
    /*get owner node*/
    struct __yx_debug_mempool_node_s_* node = *((struct __yx_debug_mempool_node_s_**)mem_offset);
    
    
    if (yx_false == _verify_the_mode(context, node, address))
    {
        /**/
    }
    else
    {
        /*remove the node from the used_list*/
        _delete_node_from_list(context->used_list, node);
        
        /*reduce the mem_size from the context*/
        context->mem_size -= node->alloced_size;
        
        
        /*free the memory*/
        yx_allocator_free(context->allocator, node->allocated_mem);
        node->mem = NULL;
        node->allocated_mem = NULL;
        node->mem_size = 0;
        node->alloced_size = 0;
        node->next = NULL;
        node->prev = NULL;
        
        
        /*release the frame info*/
        if (NULL != node->frame_address)
        {
            free(node->frame_address);
            node->frame_address = NULL;
        }
        node->frame_level = 0;
        
        
        _insert_node_to_list(context->free_list, node);
    }
    
    
}

#pragma mark static info
yx_size yx_debugMempool_memSize(yx_allocator allocator)
{
    YX_ASSERT(NULL != allocator);
    struct __yx_debug_mempool_context_s_* context = (struct __yx_debug_mempool_context_s_*)(allocator);
    
    return context->mem_size;
}

yx_size yx_debugMempool_poolSize(yx_allocator allocator)
{
    YX_ASSERT(NULL != allocator);
    struct __yx_debug_mempool_context_s_* context = (struct __yx_debug_mempool_context_s_*)(allocator);
    
    return (context->node_num * sizeof(struct __yx_debug_mempool_node_s_)) + sizeof(struct __yx_debug_mempool_context_s_) + context->mem_size;
}

yx_size yx_debugMempool_maxMemSize(yx_allocator allocator)
{
    YX_ASSERT(NULL != allocator);
    struct __yx_debug_mempool_context_s_* context = (struct __yx_debug_mempool_context_s_*)(allocator);
    
    return context->max_mem_size;
}




#pragma mark error info

yx_debugMemPool_error yx_debugMempool_lastErrorCode(yx_allocator allocator)
{
    YX_ASSERT(NULL != allocator);
    struct __yx_debug_mempool_context_s_* context = (struct __yx_debug_mempool_context_s_*)(allocator);
    
    
    return context->last_error_code;
}

yx_debugMemPool_error yx_debugMempool_checkAddress(yx_allocator allocator, yx_ptr address)
{
    if (NULL == address)
        return yx_debugMemPool_eCode_release_emptyAddress;
    
    /*context*/
    YX_ASSERT(NULL != allocator);
    struct __yx_debug_mempool_context_s_* context = (struct __yx_debug_mempool_context_s_*)(allocator);
    
    /*find the start of the memory*/
    void* mem_offset = address - sizeof(__yx_debug_mempool_guard) - sizeof(struct __yx_debug_mempool_node_s_*);
    
    /*get owner node*/
    struct __yx_debug_mempool_node_s_* node = *((struct __yx_debug_mempool_node_s_**)mem_offset);
    
    /*verify the mode*/
    _verify_the_mode(context, node, address);
    
    return context->last_error_code;
}

void yx_debugMempool_assertion(yx_allocator allocator, yx_bool is_enable)
{
    YX_ASSERT(NULL != allocator);
    struct __yx_debug_mempool_context_s_* context = (struct __yx_debug_mempool_context_s_*)(allocator);
    context->enable_assertion = is_enable;
}


void yx_debugMempool_printAllocatorStack(yx_allocator allocator)
{
    YX_ASSERT(NULL != allocator);
    struct __yx_debug_mempool_context_s_* context = (struct __yx_debug_mempool_context_s_*)(allocator);
    struct __yx_debug_mempool_node_s_* node = context->used_list->next;
    
    while (NULL != node)
    {
        yx_int num = node->frame_level;
        yx_char** frame = node->frame_address;
        
        for (yx_int i=0; i<num; i++) {
            printf("%s\n", frame[i]);
        }
        printf("\n");
        
        node = node->next;
    }
}


#pragma mark private
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

static yx_uint _expand_node_num(struct __yx_debug_mempool_context_s_* context)
{
    yx_uint expane_num = context->next_expand_num;
    
    context->next_expand_num = (expane_num >= YXDEBUGPOOL_EXPAND_MAX_NUM)? YXDEBUGPOOL_EXPAND_MAX_NUM : expane_num * 2;
    
    
    return expane_num;
}

static __yx_debug_mempool_node* _init_list(yx_allocator allocator)
{
    __yx_debug_mempool_node* node = yx_allocator_alloc(allocator, sizeof(__yx_debug_mempool_node));
    if (NULL != node) {
        node->prev = NULL;
        node->next = NULL;
        node->mem = NULL;
    }
    
    return node;
}

static void _destroy_list(yx_allocator allocator, __yx_debug_mempool_node_ref* list_pt)
{
    
    YX_ASSERT(NULL != list_pt);
    YX_ASSERT(NULL != *list_pt);
    
    __yx_debug_mempool_node* list = *list_pt;
    *list_pt = NULL;
    
    __yx_debug_mempool_node* tmpNode = list->next;
    
    while (NULL != tmpNode) {
        _delete_node_from_list(list, tmpNode);
        
        yx_allocator_free(allocator, tmpNode);
        
        tmpNode = list->next;
    }
    
    yx_allocator_free(allocator, list);
}


static void _expand_free_list(struct __yx_debug_mempool_context_s_* context)
{
    yx_uint expand_num = _expand_node_num(context);
    
    for (yx_uint i=0; i < expand_num; i++) {
        __yx_debug_mempool_node* node = yx_allocator_alloc(context->allocator, sizeof(__yx_debug_mempool_node));
        
        node->mem = NULL;
        node->mem_size = 0;
        node->allocated_mem = NULL;
        node->alloced_size = 0;
        node->frame_level = 0;
        node->frame_address = NULL;
        node->header_guard = NULL;
        node->tail_guard = NULL;
        node->next = NULL;
        node->prev = NULL;
        
        
        _insert_node_to_list(context->free_list, node);
        
        context->node_num++;
    }
    
}

static void _insert_node_to_list(__yx_debug_mempool_node* list, __yx_debug_mempool_node* node)
{
    node->next = list->next;
    if (NULL != list->next) {
        list->next->prev = node;
    }
    
    list->next = node;
    node->prev = list;
}

static void _delete_node_from_list(__yx_debug_mempool_node* list, __yx_debug_mempool_node* node)
{
    yx_unused(list);
    
    node->prev->next = node->next;
    if (NULL != node->next) {
        node->next->prev = node->prev;
    }
    
    node->next = NULL;
    node->prev = NULL;
}

static yx_bool _verify_the_mode(struct __yx_debug_mempool_context_s_* context, __yx_debug_mempool_node* node, yx_ptr address)
{
    /*reset last error code*/
    context->last_error_code = yx_debugMempool_eCode_success;
    
    
    yx_bool enable_assertion = context->enable_assertion;
    yx_bool isSuccessed = yx_false;
    
    do {
        
        
        /*vierify the double free*/
        if (NULL == node->mem  ||  NULL == node->allocated_mem ||  address != node->mem)
        {
            /*already released*/
            
            context->last_error_code = yx_debugMemPool_eCode_repeated_release;
            
            if (yx_true == enable_assertion)
                YX_ASSERT(0);
            
            break;
        }
        
        
        
        /*verify the overflow*/
        if(YX_MEMORY_MAGICNUM_HEAP_GUARD != *node->header_guard)
        {
            context->last_error_code = yx_debugMemPool_eCode_header_overflow;
            
            if (yx_true == enable_assertion)
                YX_ASSERT(0);
            break;
        }
        
        if (YX_MEMORY_MAGICNUM_HEAP_GUARD != *node->tail_guard)
        {
            context->last_error_code = yx_debugMemPool_eCode_tail_overflow;
            
            if (yx_true == enable_assertion)
                YX_ASSERT(0);
            break;
        }
    
        
        isSuccessed = yx_true;
        
    } while (0);
    
    
    return isSuccessed;
}





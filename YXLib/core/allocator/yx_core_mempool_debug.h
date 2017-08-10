//
//  yx_core_mempool_debug.h
//  YXLib
//
//  Created by Yuxi Liu on 1/5/15.
//  Copyright (c) 2015 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_MEMPOOL_DEBUG_H_
#define _YX_CORE_MEMPOOL_DEBUG_H_ 

#include "../yx_core_global.h"


/*
 Just use for debug
 */


/**
 
 Not added the yx_likely/yx_unlikely macro in debug_pool
 
 */

typedef enum yx_debugMemPool_error{
    yx_debugMempool_eCode_success = 0,
    yx_debugMemPool_eCode_leak,
    yx_debugMemPool_eCode_header_overflow,
    yx_debugMemPool_eCode_tail_overflow,
    yx_debugMemPool_eCode_repeated_release,
    yx_debugMemPool_eCode_release_emptyAddress,
}yx_debugMemPool_error;

/* basic operation */
#pragma mark basic operation

yx_allocator yx_debugMempool_create(yx_allocator allocator, yx_uint node_num);
void yx_debugMempool_destroy(yx_allocator* allocator_ptr);

void* yx_debugMempool_alloc(yx_allocator allocator, yx_size size);
void* yx_debugMempool_memalign(yx_allocator allocator, yx_size alignment, yx_size size);
void yx_debugMempool_free(yx_allocator allocator, yx_ptr address);


/* statistics info */
#pragma mark static info
yx_size yx_debugMempool_memSize(yx_allocator allocator);
yx_size yx_debugMempool_poolSize(yx_allocator allocator);
yx_size yx_debugMempool_maxMemSize(yx_allocator allocator);



/* error info */
#pragma mark error info
yx_debugMemPool_error yx_debugMempool_lastErrorCode(yx_allocator allocator);

yx_debugMemPool_error yx_debugMempool_checkAddress(yx_allocator allocator, yx_ptr address);

void yx_debugMempool_assertion(yx_allocator allocator, yx_bool is_enable);

void yx_debugMempool_printAllocatorStack(yx_allocator allocator);

#endif


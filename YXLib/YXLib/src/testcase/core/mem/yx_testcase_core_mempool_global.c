//
//  yx_testcase_core_mempool_global.c
//  YXLib
//
//  Created by Yuxi Liu on 2/10/15.
//  Copyright (c) 2015 Yuxi Liu. All rights reserved.
//

#include "yx_testcase_core_mempool_global.h"



_yx_testutility_alloc_mem _yx_testutility_allocmem_create(yx_allocator allocator, yx_irange range, yx_uint times)
{
    YX_ASSERT(NULL != allocator);
    YX_ASSERT(yx_true == YXRange_isValid(range));
    
    
    yx_allocator c_allocator = yx_stdC_allocator();
    
    struct _yx_testutility_allocmem_context* context = yx_allocator_alloc(c_allocator, sizeof(struct _yx_testutility_allocmem_context));
    
    context->mem_array = yx_allocator_alloc(c_allocator, sizeof(void**)*times);
    yx_os_memzero(context->mem_array, sizeof(void**)*times);
    
    context->size_array = yx_allocator_alloc(c_allocator, sizeof(yx_size)*times);
    yx_os_memzero(context->size_array, sizeof(yx_size)*times);
    
    context->allocator = allocator;
    context->range = range;
    context->times = times;
    context->test_readwrte = yx_false;
    
    
    return (_yx_testutility_alloc_mem)context;
}

void _yx_testutility_allocmem_destroy(_yx_testutility_alloc_mem* mem_ref)
{
    YX_ASSERT(NULL != mem_ref);
    YX_ASSERT(NULL != *mem_ref);
    
    struct _yx_testutility_allocmem_context* context = *mem_ref;
    *mem_ref = NULL;
    
    yx_allocator c_allocator = yx_stdC_allocator();
    
    yx_allocator_free(c_allocator, context->mem_array);
    yx_allocator_free(c_allocator, context->size_array);
    yx_allocator_free(c_allocator, context);
}

void _yx_testutility_allocmem_enableRWTest(_yx_testutility_alloc_mem mem, yx_bool enable)
{
    struct _yx_testutility_allocmem_context* context = (struct _yx_testutility_allocmem_context*)mem;
    
    context->test_readwrte = enable;
}

void _yx_testutility_allocmem_setAllocator(_yx_testutility_alloc_mem mem, yx_allocator allocator)
{
    struct _yx_testutility_allocmem_context* context = (struct _yx_testutility_allocmem_context*)mem;
    
    context->allocator = allocator;
}



yx_bool _yx_testutility_allocmem_alloc(_yx_testutility_alloc_mem mem)
{
    struct _yx_testutility_allocmem_context* context = (struct _yx_testutility_allocmem_context*)mem;
    yx_uint tiems = context->times;
    
    srand((unsigned int)time(NULL));
    for (yx_uint i=0; i<tiems; i++) {
        
        yx_size size = (yx_uint)(context->range.location) + ((yx_uint)rand() % (context->range.length));
        
        context->size_array[i] = size;
        context->mem_array[i] = yx_allocator_alloc(context->allocator, size);
        
        
        if (yx_true == context->test_readwrte) {
            yx_os_memset(context->mem_array[i], (size % YX_MAX_INT8), size);
        }
        
    }
    
    return yx_true;
}

yx_bool _yx_testutility_allocmem_realloc(_yx_testutility_alloc_mem mem)
{
    struct _yx_testutility_allocmem_context* context = (struct _yx_testutility_allocmem_context*)mem;
    yx_uint tiems = context->times;
    
    for (yx_uint i=0; i<tiems; i++) {
        yx_size size = context->size_array[i];
        context->mem_array[i] = yx_allocator_alloc(context->allocator, size);
        
        if (yx_true == context->test_readwrte) {
            yx_os_memset(context->mem_array[i], (size % YX_MAX_INT8), size);
        }
    }
    
    return yx_true;
}

yx_bool _yx_testutility_allocmem_memalign(_yx_testutility_alloc_mem mem)
{
    
    struct _yx_testutility_allocmem_context* context = (struct _yx_testutility_allocmem_context*)mem;
    
    /*C 开方 的 方法*/
    size_t minVal = (size_t)YXRange_MinValue(context->range);
    size_t maxVal = (size_t)YXRange_MaxValue(context->range);
    if (minVal < sizeof(void*)) {
        minVal = sizeof(void*);
    }
    
    int start = (int)((log(minVal) / log(2) ) + 0.5F);
    int end = (int)(log(maxVal) / log(2));
    
    yx_uint len = (yx_uint)((end > start)? (end + 1 - start) : 0);
    context->times = len;
    
    srand((unsigned int)time(NULL));
    for (yx_uint i=0; i<len; i++) {
        
        yx_size base = (yx_uint)(start) + ((yx_uint)rand() % (len));
        yx_size size = (yx_size)pow(2, base);
        
        context->size_array[i] = size;
        context->mem_array[i] = yx_allocator_alloc(context->allocator, size);
        
        if (yx_true == context->test_readwrte) {
            yx_os_memset(context->mem_array[i], (size % YX_MAX_INT8), size);
        }
    }
    
    return yx_true;
}

yx_bool _yx_testutility_allocmem_allocBlock(_yx_testutility_alloc_mem mem)
{
    struct _yx_testutility_allocmem_context* context = (struct _yx_testutility_allocmem_context*)mem;
    
    yx_uint start = (yx_uint)(context->range.location);
    yx_uint end = (yx_uint)(YXRange_MaxValue(context->range));
    end -= end % YX_ALIGNMENT;
    
    yx_int length = (yx_int)(end - start);
    if (length < 0)
        return yx_false;
    
    yx_uint times = context->times;
    
    srand((unsigned int)time(NULL));
    for (yx_uint i=0; i<times; i++) {
        
        yx_size size = start + ((yx_uint)rand() % (yx_uint)length);
//        size += YX_ALIGNMENT - (size % YX_ALIGNMENT);
        
        size = (size + YX_ALIGNMENT - 1) & ~((yx_ulong)YX_ALIGNMENT - 1);
        
        
        context->size_array[i] = size;
        context->mem_array[i] = yx_allocator_alloc(context->allocator, size);
        
        
        if (yx_true == context->test_readwrte) {
            yx_os_memset(context->mem_array[i], (size % YX_MAX_INT8), size);
        }
        
    }
    
    
    return yx_true;
}

yx_bool _yx_testutility_allocmem_free(_yx_testutility_alloc_mem mem)
{
    struct _yx_testutility_allocmem_context* context = (struct _yx_testutility_allocmem_context*)mem;
    yx_uint tiems = context->times;
    
    for (yx_uint i=0; i<tiems; i++) {
        yx_allocator_free(context->allocator, context->mem_array[i]);
    }
    
    return yx_true;
}

yx_bool _yx_testutility_allocmem_test(_yx_testutility_alloc_mem mem)
{
    struct _yx_testutility_allocmem_context* context = (struct _yx_testutility_allocmem_context*)mem;
    yx_uint tiems = context->times;
    yx_allocator c_allocator = yx_stdC_allocator();
    
    for (yx_uint i=0; i<tiems; i++) {
        void* pt = context->mem_array[i];
        
        if (NULL == pt)
            goto errout;
        
        if (0 != ((yx_size)pt % YX_ALIGNMENT))
            goto errout;
        
        
        if (yx_true == context->test_readwrte)
        {
            yx_size size = context->size_array[i];
            void* target = yx_allocator_alloc(c_allocator, size);
            yx_os_memset(target, (size % YX_MAX_INT8), size);
            
            if (0 != yx_os_memcmp(target, pt, size))
                goto errout;
            
        }
        

    }
    
    return yx_true;
    
errout:
    return yx_false;
}

yx_size _yx_testutility_allocmem_totleSize(_yx_testutility_alloc_mem mem)
{
    
    struct _yx_testutility_allocmem_context* context = (struct _yx_testutility_allocmem_context*)mem;
    yx_uint tiems = context->times;
    
    yx_size totle_size = 0;
    for (yx_uint i=0; i<tiems; i++) {
        totle_size += context->size_array[i];
    }
    
    return totle_size;
}



//yx_bool _yx_testutility_check_allocator2(yx_allocator allocator)
//{
//    
//    void* mem_arr[1024];
//    void* buffer[1024];
//    
//    
//    /*check handle*/
//    
//    if (NULL == allocator)
//        goto errout;
//    
//    yx_allocator_contex_ref context = allocator;
//    if (NULL == context->handle)
//        goto errout;
//    
//    if (NULL == context->alloc)
//        goto  errout;
//    
//    if (NULL == context->free)
//        goto errout;
//    
//    
//    /*check memory and release*/
//    
//    yx_os_memzero(mem_arr, sizeof(void*) * 1024);
//    
//    for (yx_uint i=0; i<1024; i++){
//        mem_arr[i] = yx_allocator_alloc(allocator, i);
//        if (NULL == mem_arr[i])
//            goto errout;
//        
//        yx_os_memset(mem_arr[i], (i % YX_MAX_INT8), i);
//    }
//    
//    for (yx_uint i=0; i<1024; i++) {
//        
//        if (NULL == mem_arr[i]){
//            goto errout;
//        }
//        
//        if (0 != ((yx_size)mem_arr[i] % YX_ALIGNMENT)) {
//            goto errout;
//        }
//        
//        
//        yx_os_memset(buffer, (i % YX_MAX_INT8), i);
//        if (0 != yx_os_memcmp(buffer, mem_arr[i], i)) {
//            goto errout;
//        }
//    }
//    
//    for (yx_uint i=0; i<1024; i++) {
//        yx_allocator_free(allocator, mem_arr[i]);
//    }
//
//    
//    
//    
//    
//    /*check memalignment*/
//    
//    yx_uint startTest = (yx_uint)sizeof(void*);
//    
//    yx_os_memzero(mem_arr, sizeof(void*) * 1024);
//    
//    for (yx_uint i=startTest; i<1024; i*=2){
//        mem_arr[i] = yx_allocator_memalign(allocator, i, i);
//        if (NULL == mem_arr[i])
//            goto errout;
//        yx_os_memset(mem_arr[i], (i % YX_MAX_INT8), i);
//    }
//    
//    for (yx_uint i=startTest; i<1024; i*=2) {
//        
//        if (NULL == mem_arr[i]){
//            goto errout;
//        }
//        
//        if (0 != ((yx_size)mem_arr[i] % i)) {
//            goto errout;
//        }
//        
//        
//        yx_os_memset(buffer, (i % YX_MAX_INT8), i);
//        if (0 != yx_os_memcmp(buffer, mem_arr[i], i)) {
//            goto errout;
//        }
//    }
//    
//    for (yx_uint i=startTest; i<1024; i*=2) {
//        yx_allocator_free(allocator, mem_arr[i]);
//    }
//    
//
//    
//    return yx_true;
//    
//errout:
//    return yx_false;
//}


yx_bool _yx_testutility_check_allocator(yx_allocator allocator)
{
    
    /*check handle*/
    
    if (NULL == allocator)
        goto errout;
    
    yx_allocator_contex_ref context = allocator;
    if (NULL == context->handle)
        goto errout;
    
    if (NULL == context->alloc)
        goto  errout;
    
    if (NULL == context->free)
        goto errout;
    
    
    /*check memory and release*/
    _yx_testutility_alloc_mem normalMem = _yx_testutility_allocmem_create(allocator, YXRange_make(0, 1024), 10240);
    _yx_testutility_allocmem_enableRWTest(normalMem, yx_true);
    
    _yx_testutility_allocmem_alloc(normalMem);
    
    if (yx_false == _yx_testutility_allocmem_test(normalMem))
        goto errout;
    
    _yx_testutility_allocmem_free(normalMem);
    _yx_testutility_allocmem_destroy(&normalMem);
    
    
    
    /*check alignment function*/
    _yx_testutility_alloc_mem alignment = _yx_testutility_allocmem_create(allocator, YXRange_make(0, 1024), 10240);
    _yx_testutility_allocmem_enableRWTest(alignment, yx_true);
    
    _yx_testutility_allocmem_memalign(alignment);
    
    if (yx_false == _yx_testutility_allocmem_test(alignment))
        goto errout;
    
    _yx_testutility_allocmem_free(alignment);
    _yx_testutility_allocmem_destroy(&alignment);
    
    

    
    return yx_true;
    
errout:
    return yx_false;
}

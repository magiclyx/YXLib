//
//  yx_testcase_core_mempool_debug.c
//  YXLib
//
//  Created by Yuxi Liu on 2/10/15.
//  Copyright (c) 2015 Yuxi Liu. All rights reserved.
//

#include "yx_testcase_core_mempool_debug.h"

static yx_unittest_group _g_ut_group_mempool_debug = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////
static YX_UTEST_FUN(_test_basic_function_);
static YX_UTEST_FUN(_test_overflow_detection_);
static YX_UTEST_FUN(_test_leak_detection_);
static YX_UTEST_FUN(_test_double_release_);
static YX_UTEST_FUN(_test_size_stable_);

////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma mark setup/shutdown
yx_bool yx_testcase_core_mempool_debug_setup(yx_unittest ut, yx_allocator allocator)
{
    yx_unused(ut);
    
    _g_ut_group_mempool_debug = yx_ut_create_group(allocator, "mempool_debug");
    YX_ASSERT(NULL != _g_ut_group_mempool_debug);
    
    yx_ut_group_add_case(_g_ut_group_mempool_debug, "basic function", _test_basic_function_);
    yx_ut_group_add_case(_g_ut_group_mempool_debug, "overflow detection", _test_overflow_detection_);
    yx_ut_group_add_case(_g_ut_group_mempool_debug, "memoryleak detection", _test_leak_detection_);
    yx_ut_group_add_case(_g_ut_group_mempool_debug, "double release", _test_double_release_);
    yx_ut_group_add_case(_g_ut_group_mempool_debug, "pool size stable test", _test_size_stable_);
    
    yx_ut_add_group(ut, _g_ut_group_mempool_debug);
    
    return yx_true;
}

yx_bool yx_testcase_core_mempool_debug_shutdown(yx_unittest ut)
{
    yx_unused(ut);
    
    yx_ut_destroy_group(&_g_ut_group_mempool_debug);
    return yx_true;
}

#pragma mark test case
static YX_UTEST_FUN(_test_basic_function_)
{
    yx_allocator allocator = yx_debugMempool_create(NULL, 0);
    YX_UTEST_VERIFY(yx_true == _yx_testutility_check_allocator(allocator), "");
    yx_debugMempool_destroy(&allocator);
}

static YX_UTEST_FUN(_test_overflow_detection_)
{
    yx_allocator allocator = yx_debugMempool_create(NULL, 0);
    yx_debugMempool_assertion(allocator, yx_false);
    
    void* pt = NULL;
    void* pt_alignment = NULL;
    yx_char* test_pt = NULL;
    yx_char* test_alignment_pt = NULL;
    
    pt = yx_allocator_alloc(allocator, 16);
    pt_alignment = yx_allocator_memalign(allocator, 2 * YX_ALIGNMENT, 16);
    
    /*not overflow*/
    YX_UTEST_VERIFY(yx_debugMempool_eCode_success == yx_debugMempool_checkAddress(allocator, pt), "");
    YX_UTEST_VERIFY(yx_debugMempool_eCode_success == yx_debugMempool_checkAddress(allocator, pt_alignment), "");
    
    
    /*tail overflow*/
    test_pt = pt + 16;
    *test_pt = 'a';
    YX_UTEST_VERIFY(yx_debugMemPool_eCode_tail_overflow == yx_debugMempool_checkAddress(allocator, pt), "");
    
    test_alignment_pt = pt_alignment + 16;
    *test_alignment_pt = 'a';
    YX_UTEST_VERIFY(yx_debugMemPool_eCode_tail_overflow == yx_debugMempool_checkAddress(allocator, pt_alignment), "");
    
    
    /*head overflow*/
    test_pt = pt - 1;
    *test_pt = 'a';
    YX_UTEST_VERIFY(yx_debugMemPool_eCode_header_overflow == yx_debugMempool_checkAddress(allocator, pt), "");
    
    test_alignment_pt = pt_alignment - 1;
    *test_alignment_pt = 'a';
    YX_UTEST_VERIFY(yx_debugMemPool_eCode_header_overflow == yx_debugMempool_checkAddress(allocator, pt_alignment), "");
    
    
    yx_allocator_free(allocator, pt);
    yx_allocator_free(allocator, pt_alignment);
    
    yx_debugMempool_destroy(&allocator);
}

static YX_UTEST_FUN(_test_leak_detection_)
{
    yx_allocator allocator = yx_debugMempool_create(NULL, 0);
    yx_debugMempool_assertion(allocator, yx_false);
    
    void* pt1 = NULL;
    void* pt2 = NULL;
    void* pt3 = NULL;
    void* pt4 = NULL;
    
    pt1 = yx_allocator_alloc(allocator, 8);
    pt2 = yx_allocator_memalign(allocator, 2*YX_ALIGNMENT, 8);
    pt3 = yx_allocator_alloc(allocator, 8);
    pt4 = yx_allocator_alloc(allocator, 16);
    
    yx_allocator_free(allocator, pt3);
    yx_allocator_free(allocator, pt4);
    
    pt3 = yx_allocator_alloc(allocator, 8);
    pt4 = yx_allocator_alloc(allocator, 16);
    
    yx_allocator_free(allocator, pt3);
    yx_allocator_free(allocator, pt4);
    
    YX_UTEST_VERIFY(16 == yx_debugMempool_memSize(allocator), "");
    
    
    yx_allocator_free(allocator, pt1);
    yx_allocator_free(allocator, pt2);
    
    yx_debugMempool_destroy(&allocator);
}



static YX_UTEST_FUN(_test_double_release_)
{
    void* pt = NULL;
    
    yx_allocator allocator = yx_debugMempool_create(NULL, 0);
    yx_debugMempool_assertion(allocator, yx_false);
    
    pt = yx_allocator_alloc(allocator, 8);
    yx_allocator_free(allocator, pt);
    YX_UTEST_VERIFY(yx_debugMempool_eCode_success == yx_debugMempool_lastErrorCode(allocator), "");
    yx_allocator_free(allocator, pt);
    YX_UTEST_VERIFY(yx_debugMemPool_eCode_repeated_release == yx_debugMempool_lastErrorCode(allocator), "");
    
    yx_debugMempool_destroy(&allocator);
}

static YX_UTEST_FUN(_test_size_stable_)
{
    
    yx_allocator allocator = yx_debugMempool_create(NULL, 1024);
    yx_debugMempool_assertion(allocator, yx_false);
    
    yx_size size;
    
    _yx_testutility_alloc_mem mem = _yx_testutility_allocmem_create(allocator, YXRange_make(0, 1024), 4096);
    
    _yx_testutility_allocmem_alloc(mem);
    _yx_testutility_allocmem_free(mem);
    
    
    /*record mem size*/
    size = yx_debugMempool_poolSize(allocator);
    
    
    for (int count = 0; count < 30; count++) {
        _yx_testutility_allocmem_realloc(mem);
        _yx_testutility_allocmem_free(mem);
    }
    
    _yx_testutility_allocmem_destroy(&mem);
    
    
    /*verify the pool size stable*/
    YX_UTEST_VERIFY(size == yx_debugMempool_poolSize(allocator), "");
    
    
    yx_debugMempool_destroy(&allocator);

}



//
//  yx_testcase_core_mempool_basic.c
//  YXLib
//
//  Created by Yuxi Liu on 2/13/15.
//  Copyright (c) 2015 Yuxi Liu. All rights reserved.
//

#include "yx_testcase_core_mempool_basic.h"

static yx_unittest_group _g_ut_group_mempool_basic;

////////////////////////////////////////////////////////////////////////////////////////////////////
static YX_UTEST_FUN(_test_basic_function_);
static YX_UTEST_FUN(_test_large_block_);
static YX_UTEST_FUN(_test_size_stable_);
static YX_UTEST_FUN(_test_leak_detection_);
static YX_UTEST_FUN(_test_time_performance);
static YX_UTEST_FUN(_test_space_performance);
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma mark #pragma mark setup/shutdown
yx_bool yx_testcase_core_mempool_basic_setup(yx_unittest ut, yx_allocator allocator)
{
    yx_unused(ut);
    
    _g_ut_group_mempool_basic = yx_ut_create_group(allocator, "mempool_basic");
    YX_ASSERT(NULL != _g_ut_group_mempool_basic);
    
    yx_ut_group_add_case(_g_ut_group_mempool_basic, "basic function", _test_basic_function_);
    yx_ut_group_add_case(_g_ut_group_mempool_basic, "large block", _test_large_block_);
    yx_ut_group_add_case(_g_ut_group_mempool_basic, "pool size stable test", _test_size_stable_);
    yx_ut_group_add_case(_g_ut_group_mempool_basic, "memoryleak detection", _test_leak_detection_);
    yx_ut_group_add_case(_g_ut_group_mempool_basic, "time performance", _test_time_performance);
    yx_ut_group_add_case(_g_ut_group_mempool_basic, "space performance", _test_space_performance);

    yx_ut_add_group(ut, _g_ut_group_mempool_basic);

    
    return yx_true;
}

yx_bool yx_testcase_core_mempool_basic_shutdown(yx_unittest ut)
{
    yx_unused(ut);
    
    yx_ut_destroy_group(&_g_ut_group_mempool_basic);
    return yx_true;
}


#pragma mark testcase

static YX_UTEST_FUN(_test_basic_function_)
{
    yx_allocator allocator = yx_basicMempool_create(NULL);
    YX_UTEST_VERIFY(yx_true == _yx_testutility_check_allocator(allocator), "");
    yx_basicMempool_destroy(&allocator);
}

static YX_UTEST_FUN(_test_large_block_)
{
    yx_allocator allocator = yx_basicMempool_create(NULL);
    
    _yx_testutility_alloc_mem mem = _yx_testutility_allocmem_create(allocator, YXRange_make(1024*15, 1024*17), 1024);
    _yx_testutility_allocmem_enableRWTest(mem, yx_true);
    _yx_testutility_allocmem_alloc(mem);
    
    YX_UTEST_VERIFY(yx_true == _yx_testutility_allocmem_test(mem), "");
    
    _yx_testutility_allocmem_free(mem);
    _yx_testutility_allocmem_destroy(&mem);
    
    
    yx_basicMempool_destroy(&allocator);
}

static YX_UTEST_FUN(_test_size_stable_)
{
    yx_allocator debug_allocator = yx_debugMempool_create(NULL, 4096);
    yx_allocator allocator = yx_basicMempool_create(debug_allocator);
    _yx_testutility_alloc_mem mem_normal = _yx_testutility_allocmem_create(allocator, YXRange_make(0, 4096), 4096);
    _yx_testutility_alloc_mem mem_large = _yx_testutility_allocmem_create(allocator, YXRange_make(1024*16, 1024*17), 1024);
    
    
    _yx_testutility_allocmem_alloc(mem_normal);
    _yx_testutility_allocmem_alloc(mem_large);
    _yx_testutility_allocmem_free(mem_normal);
    _yx_testutility_allocmem_free(mem_large);
    yx_size size = yx_debugMempool_memSize(debug_allocator);
    
    
    for (int count = 0; count < 30; count++) {
        _yx_testutility_allocmem_realloc(mem_normal);
        _yx_testutility_allocmem_realloc(mem_large);
        _yx_testutility_allocmem_free(mem_normal);
        _yx_testutility_allocmem_free(mem_large);
        
        YX_UTEST_VERIFY(yx_debugMempool_memSize(debug_allocator) == size, "mem used not stable");
    }
    
    
    _yx_testutility_allocmem_destroy(&mem_normal);
    _yx_testutility_allocmem_destroy(&mem_large);
    
    yx_basicMempool_destroy(&allocator);
    yx_debugMempool_destroy(&debug_allocator);
    
}

static YX_UTEST_FUN(_test_leak_detection_)
{
    /*init the debug pool*/
    yx_allocator debug_allocator = yx_debugMempool_create(NULL, 4096);
    yx_debugMempool_assertion(debug_allocator, yx_false);
    
    /*init the local pool*/
    yx_allocator allocator = yx_basicMempool_create(debug_allocator);
    
    /*init the mem*/
    _yx_testutility_alloc_mem mem_normal = _yx_testutility_allocmem_create(allocator, YXRange_make(0, 4096), 4096);
    _yx_testutility_alloc_mem mem_large = _yx_testutility_allocmem_create(allocator, YXRange_make(1024*16, 1024*17), 1024);
    
   // _yx_testutility_alloc_mem mem = _yx_testutility_allocmem_create(allocator, YXRange_make(0, 128), 4096);
    
    /*alloc and free the memory*/
    _yx_testutility_allocmem_alloc(mem_normal);
    _yx_testutility_allocmem_free(mem_normal);
    _yx_testutility_allocmem_alloc(mem_large);
    _yx_testutility_allocmem_free(mem_large);
    
    /*destroy the mem*/
    _yx_testutility_allocmem_destroy(&mem_normal);
    _yx_testutility_allocmem_destroy(&mem_large);
    
    /*destroy the local mem*/
    yx_basicMempool_destroy(&allocator);
    
    
    yx_debugMempool_printAllocatorStack(debug_allocator);
    
    /*test leak*/
    YX_UTEST_VERIFY(0 == yx_debugMempool_memSize(debug_allocator), "mem leak");
    
    /*free the debug pool*/
    yx_debugMempool_destroy(&debug_allocator);
}

static YX_UTEST_FUN(_test_time_performance)
{
    yx_os_tick start;
    yx_os_tick end;
    
    yx_real stdc_time_used;
    yx_real basicpool_time_used;
    
    yx_allocator c_allocator = yx_stdC_allocator();
    yx_allocator basic_allocator = yx_basicMempool_create(c_allocator);
    
    _yx_testutility_alloc_mem mem = _yx_testutility_allocmem_create(c_allocator, YXRange_make(0, yx_basicMempool_maxSlotSize()), 40960);
    //_yx_testutility_allocmem_enableRWTest(mem, yx_true);
    
    
    /*warmup*/
    _yx_testutility_allocmem_alloc(mem);
    _yx_testutility_allocmem_free(mem);
    
    /*test stdc malloc*/
    start = yx_os_ticket_now();
    for (int count = 0; count < 30; count++) {
        _yx_testutility_allocmem_realloc(mem);
        _yx_testutility_allocmem_free(mem);
    }
    end = yx_os_ticket_now();
    stdc_time_used = yx_os_ticket_toNanoSecond(end - start);
    
    
    _yx_testutility_allocmem_setAllocator(mem, basic_allocator);
    
    /*warmup*/
    _yx_testutility_allocmem_realloc(mem);
    _yx_testutility_allocmem_free(mem);
    
    /*test middle mem pool*/
    start = yx_os_ticket_now();
    for (int count = 0; count < 30; count++) {
        _yx_testutility_allocmem_realloc(mem);
        _yx_testutility_allocmem_free(mem);
    }
    end = yx_os_ticket_now();
    basicpool_time_used = yx_os_ticket_toNanoSecond(end - start);
    
    _yx_testutility_allocmem_destroy(&mem);
    yx_basicMempool_destroy(&basic_allocator);
    
    
    YX_UTEST_VERIFY(stdc_time_used / basicpool_time_used > 19.5f, "time out");
}

static YX_UTEST_FUN(_test_space_performance)
{
    yx_allocator debug_allocator = yx_debugMempool_create(NULL, 4096);
    yx_debugMempool_assertion(debug_allocator, yx_false);
    
    /*init the local pool*/
    yx_allocator allocator = yx_basicMempool_create(debug_allocator);
    
    /*init the mem*/
    _yx_testutility_alloc_mem mem = _yx_testutility_allocmem_create(allocator, YXRange_make(0, 1024*16), 40960);
    
    
    /*alloc and free the memory*/
    _yx_testutility_allocmem_allocBlock(mem);
    
    yx_size mem_wanted = _yx_testutility_allocmem_totleSize(mem);
    yx_size mem_used = yx_debugMempool_memSize(debug_allocator);
//    yx_size mem_waist = yx_localMempool_waistSize(allocator);
//    yx_size mem_remain = yx_localMempool_RemainSize(allocator);
//    yx_size mem_struct = yx_localMempool_structSize(allocator);
    
    
    /*destroy the mem*/
    _yx_testutility_allocmem_destroy(&mem);
    
    /*destroy the local mem*/
    yx_basicMempool_destroy(&allocator);
    
    /*free the debug pool*/
    yx_debugMempool_destroy(&debug_allocator);
    
    
    YX_UTEST_VERIFY(((yx_real)(mem_wanted) / (yx_real)mem_used) > 0.90, "mem used too large");
}


//
//  yxut_core_mempool_buf.c
//  YXLib
//
//  Created by Yuxi Liu on 11/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#include "yx_testcase_core_mempool_buf.h"

static yx_unittest_group _g_ut_group_mempool_buf;

////////////////////////////////////////////////////////////////////////////////////////////////////
static YX_UTEST_FUN(_test_basic_function_);
static YX_UTEST_FUN(_test_large_block_);
static YX_UTEST_FUN(_test_size_stable_);
static YX_UTEST_FUN(_test_leak_detection_);
static YX_UTEST_FUN(_test_time_performance);
static YX_UTEST_FUN(_test_space_performance);
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma mark #pragma mark setup/shutdown
yx_bool yx_testcase_core_mempool_buf_setup(yx_unittest ut, yx_allocator allocator)
{
    yx_unused(ut);
    
    _g_ut_group_mempool_buf = yx_ut_create_group(allocator, "mempool_buffer");
    YX_ASSERT(NULL != _g_ut_group_mempool_buf);
    
    yx_ut_group_add_case(_g_ut_group_mempool_buf, "basic function", _test_basic_function_);
    yx_ut_group_add_case(_g_ut_group_mempool_buf, "large block", _test_large_block_);
    yx_ut_group_add_case(_g_ut_group_mempool_buf, "pool size stable test", _test_size_stable_);
    yx_ut_group_add_case(_g_ut_group_mempool_buf, "memoryleak detection", _test_leak_detection_);
    yx_ut_group_add_case(_g_ut_group_mempool_buf, "time performance", _test_time_performance);
    yx_ut_group_add_case(_g_ut_group_mempool_buf, "space performance", _test_space_performance);

    yx_ut_add_group(ut, _g_ut_group_mempool_buf);
    
    return yx_true;
}

yx_bool yx_testcase_core_mempool_buf_shutdown(yx_unittest ut)
{
    yx_unused(ut);
    
    yx_ut_destroy_group(&_g_ut_group_mempool_buf);
    return yx_true;
}




#pragma mark testcase
static YX_UTEST_FUN(_test_basic_function_)
{
    yx_allocator allocator = yx_bufMempool_create(NULL, 1024);
    YX_UTEST_VERIFY(yx_true == _yx_testutility_check_allocator(allocator), "");
    yx_bufMempool_reset(allocator);
    yx_bufMempool_destroy(&allocator);
}

static YX_UTEST_FUN(_test_large_block_)
{
    yx_allocator allocator = yx_bufMempool_create(NULL, 64);
    
    
    _yx_testutility_alloc_mem mem = _yx_testutility_allocmem_create(allocator, YXRange_make(0, 4096), 4019);
    _yx_testutility_allocmem_enableRWTest(mem, yx_true);
    _yx_testutility_allocmem_alloc(mem);
    
    YX_UTEST_VERIFY(yx_true == _yx_testutility_allocmem_test(mem), "");
    
    _yx_testutility_allocmem_free(mem);
    _yx_testutility_allocmem_destroy(&mem);
    
    
    yx_bufMempool_reset(allocator);
    yx_bufMempool_destroy(&allocator);
}

static YX_UTEST_FUN(_test_size_stable_)
{
    yx_allocator debug_allocator = yx_debugMempool_create(NULL, 4096);
    yx_allocator allocator = yx_bufMempool_create(debug_allocator, 1024);
    _yx_testutility_alloc_mem mem = _yx_testutility_allocmem_create(allocator, YXRange_make(0, 2048), 4096);
    
    _yx_testutility_allocmem_alloc(mem);
    _yx_testutility_allocmem_free(mem);
    yx_size size = yx_debugMempool_memSize(debug_allocator);
    yx_int block_num = yx_bufMempool_blockNum(allocator);
    yx_int largeBlock_num = yx_bufMempool_largeBlockUsedNum(allocator);
    yx_int freeLargeBlock_num = yx_bufMempool_largeblockFreeNum(allocator);
    
    yx_bufMempool_reset(allocator);
    
    for (int count = 0; count < 30; count++) {
        _yx_testutility_allocmem_realloc(mem);
        _yx_testutility_allocmem_free(mem);
        
        YX_UTEST_VERIFY(yx_debugMempool_memSize(debug_allocator) == size, "mem used not stable");
        YX_UTEST_VERIFY(yx_bufMempool_blockNum(allocator) == block_num, "block num not stable");
        YX_UTEST_VERIFY(yx_bufMempool_largeBlockUsedNum(allocator) == largeBlock_num, "large block num not stable");
        YX_UTEST_VERIFY(yx_bufMempool_largeblockFreeNum(allocator) == freeLargeBlock_num, "large block num not stable");
        
        yx_bufMempool_reset(allocator);
    }
    
    
    
    _yx_testutility_allocmem_destroy(&mem);
    yx_bufMempool_destroy(&allocator);
    yx_debugMempool_destroy(&debug_allocator);
}

static YX_UTEST_FUN(_test_leak_detection_)
{
    /*init the debug pool*/
    yx_allocator debug_allocator = yx_debugMempool_create(NULL, 4096);
    yx_debugMempool_assertion(debug_allocator, yx_false);
    
    /*init the buf pool*/
    yx_allocator allocator = yx_bufMempool_create(debug_allocator, 1024);
    
    /*init the mem*/
    _yx_testutility_alloc_mem mem = _yx_testutility_allocmem_create(allocator, YXRange_make(0, 2048), 4096);
    
    /*alloc and free the memory*/
    _yx_testutility_allocmem_alloc(mem);
    _yx_testutility_allocmem_free(mem);
    
    
    /*destroy the mem*/
    _yx_testutility_allocmem_destroy(&mem);
    
    /*destroy the buf mem*/
    yx_bufMempool_destroy(&allocator);
    
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
    yx_real buf_time_used;
    yx_real buf_non_free_time_used;
    
    yx_allocator c_allocator = yx_stdC_allocator();
    yx_allocator buf_allocator = yx_bufMempool_create(c_allocator, 4096);
    
    _yx_testutility_alloc_mem mem = _yx_testutility_allocmem_create(c_allocator, YXRange_make(0, 64), 40960);
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
    
    
    _yx_testutility_allocmem_setAllocator(mem, buf_allocator);
    
    /*warmup*/
    _yx_testutility_allocmem_realloc(mem);
    _yx_testutility_allocmem_free(mem);
    yx_bufMempool_reset(buf_allocator);
    
    /*test buf mem pool*/
    start = yx_os_ticket_now();
    for (int count = 0; count < 30; count++) {
        _yx_testutility_allocmem_realloc(mem);
        _yx_testutility_allocmem_free(mem);
        yx_bufMempool_reset(buf_allocator);
    }
    end = yx_os_ticket_now();
    buf_time_used = yx_os_ticket_toNanoSecond(end - start);
    
    
    /*test buf mem pool with no free function*/
    start = yx_os_ticket_now();
    for (int count = 0; count < 30; count++) {
        _yx_testutility_allocmem_realloc(mem);
        yx_bufMempool_reset(buf_allocator);
    }
    end = yx_os_ticket_now();
    buf_non_free_time_used = yx_os_ticket_toNanoSecond(end - start);
    
    
    YX_UTEST_VERIFY(stdc_time_used / buf_time_used > 5.5f, "time out");
    YX_UTEST_VERIFY(stdc_time_used / buf_non_free_time_used > 9.0f, "time out");
    
    
    _yx_testutility_allocmem_destroy(&mem);
    
    yx_bufMempool_destroy(&buf_allocator);
}

static YX_UTEST_FUN(_test_space_performance)
{
    yx_allocator debug_allocator = yx_debugMempool_create(NULL, 4096);
    yx_debugMempool_assertion(debug_allocator, yx_false);
    
    /*init the buf pool*/
    yx_allocator allocator = yx_bufMempool_create(debug_allocator, 4096);
    
    /*init the mem*/
    _yx_testutility_alloc_mem mem = _yx_testutility_allocmem_create(allocator, YXRange_make(0, 64), 4096);
    
    
    /*alloc and free the memory*/
    _yx_testutility_allocmem_allocBlock(mem);
    
    /*
     因为没有办法记录从large_block中分出的内存，所以设置测试内存时，不要从largeBlock中分配
     */
    yx_size mem_wanted = _yx_testutility_allocmem_totleSize(mem);
    yx_size mem_used = yx_debugMempool_memSize(debug_allocator);
    yx_size mem_waist = yx_bufMempool_waistSpace(allocator);
    yx_size mem_remain = yx_bufMempool_remainSpace(allocator);
    yx_size mem_struct = yx_bufMempool_structureSpace(allocator);
    
    
    /*destroy the mem*/
    _yx_testutility_allocmem_destroy(&mem);
    
    /*destroy the buf mem*/
    yx_bufMempool_destroy(&allocator);
    
    /*test leak*/
    YX_UTEST_VERIFY(0 == yx_debugMempool_memSize(debug_allocator), "mem leak");
    
    /*free the debug pool*/
    yx_debugMempool_destroy(&debug_allocator);
    
    YX_UTEST_VERIFY(((yx_real)(mem_wanted + mem_remain) / (yx_real)mem_used) > 0.985, "mem used too large");
    YX_UTEST_VERIFY(((yx_real)(mem_wanted + mem_remain + mem_waist + mem_struct) / (yx_real)mem_used) == 1.0, "mem missed");
 
}


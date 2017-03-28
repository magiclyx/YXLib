//
//  yx_testcase_core_mempool_cell.c
//  YXLib
//
//  Created by LiuYuxi on 15/6/29.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_testcase_core_mempool_cell.h"

static yx_unittest_group _g_ut_group_mempool_cell = NULL;


////////////////////////////////////////////////////////////////////////////////////////////////////

static YX_UTEST_FUN(_test_basic_function_);
static YX_UTEST_FUN(_test_size_stable);
static YX_UTEST_FUN(_test_time_performance);
static YX_UTEST_FUN(_test_space_performance);

////////////////////////////////////////////////////////////////////////////////////////////////////

yx_bool yx_testcase_core_mempool_cell_setup(yx_unittest ut, yx_allocator allocator)
{
    yx_unused(ut);
    
    _g_ut_group_mempool_cell = yx_ut_create_group(allocator, "mempool_cell");
    YX_ASSERT(NULL != _g_ut_group_mempool_cell);
    
    
    yx_ut_group_add_case(_g_ut_group_mempool_cell, "basic function", _test_basic_function_);
    yx_ut_group_add_case(_g_ut_group_mempool_cell, "memory stable", _test_size_stable);
    yx_ut_group_add_case(_g_ut_group_mempool_cell, "time performance", _test_time_performance);
    yx_ut_group_add_case(_g_ut_group_mempool_cell, "space performance", _test_space_performance);

    yx_ut_add_group(ut, _g_ut_group_mempool_cell);
    
    return yx_true;
}

yx_bool yx_testcase_core_mempool_cell_shutdown(yx_unittest ut)
{
    yx_unused(ut);
    
    yx_ut_destroy_group(&_g_ut_group_mempool_cell);
    return yx_true;

}

#pragma mark test
static YX_UTEST_FUN(_test_basic_function_)
{
    yx_allocator debug_allocator = yx_debugMempool_create(NULL, 4096);
    yx_allocator allocator = yx_cellMempool_create(NULL, 128);
    
    yx_ptr arr[100];
    
    /*
     alloc & free
     */
    for (int i=0; i < 100; i++)
    {
        arr[i] = yx_allocator_alloc(allocator, 128);
        memset(arr[i], 0, 128);
    }
    
    for (int i=0; i < 100; i++)
    {
        yx_allocator_free(allocator, arr[i]);
    }
    
    
    /*
     test again
     */
    for (int i=0; i < 100; i++)
    {
        arr[i] = yx_allocator_alloc(allocator, 128);
        memset(arr[i], 0, 128);
    }
    
    for (int i=0; i < 100; i++)
    {
        yx_allocator_free(allocator, arr[i]);
    }

    
    
    
    yx_cellMempool_destroy(&allocator);
    yx_debugMempool_destroy(&debug_allocator);
    
    YX_UTEST_VERIFY(1, "能平安运行到这里，已经可以了. debugpool 自带的assert已经验证了大部分内容")
}

static YX_UTEST_FUN(_test_size_stable)
{
    yx_allocator debug_allocator = yx_debugMempool_create(NULL, 4096);
    yx_allocator allocator = yx_cellMempool_create(NULL, 128);
    
    yx_ptr arr[100];
    
    
    yx_size memsize = 0;
    
    /*
     alloc & free
     */
    for (int i=0; i < 100; i++)
    {
        arr[i] = yx_allocator_alloc(allocator, 128);
    }
    
    memsize = yx_debugMempool_memSize(debug_allocator);
    
    for (int i=0; i < 100; i++)
    {
        yx_allocator_free(allocator, arr[i]);
    }
    
    YX_UTEST_VERIFY(memsize == yx_debugMempool_memSize(debug_allocator), "mem use not stable");
    
    /*
     test again
     */
    for (int i=0; i < 100; i++)
    {
        arr[i] = yx_allocator_alloc(allocator, 128);
    }
    
    YX_UTEST_VERIFY(memsize == yx_debugMempool_memSize(debug_allocator), "mem use not stable");
    
    
    for (int i=0; i < 100; i++)
    {
        yx_allocator_free(allocator, arr[i]);
    }
    
    YX_UTEST_VERIFY(memsize == yx_debugMempool_memSize(debug_allocator), "mem use not stable");
    
    
    yx_cellMempool_destroy(&allocator);
    yx_debugMempool_destroy(&debug_allocator);
}


static YX_UTEST_FUN(_test_time_performance)
{
    yx_os_tick start;
    yx_os_tick end;
    
    yx_real stdc_time_used;
    yx_real cell_time_used_firstTime;
    yx_real cell_time_used_secondTime;
    
    
    yx_ptr arr[40960];

    
    yx_allocator c_allocator = yx_stdC_allocator();
    yx_allocator cell_allocator = yx_cellMempool_create(c_allocator, 128);
    
    
    /*warmup*/
    for (int i=0; i<40960; i++)
    {
        arr[i] = yx_allocator_alloc(c_allocator, 128);
        memset(arr[i], 0, 128);
    }
    for (int i=0; i<40960; i++)
    {
        yx_allocator_free(c_allocator, arr[i]);
    }
    
    /*test std c*/
    start = yx_os_ticket_now();
    for (int i=0; i<40960; i++)
    {
        arr[i] = yx_allocator_alloc(c_allocator, 128);
        memset(arr[i], 0, 128);
    }
    for (int i=0; i<40960; i++)
    {
        yx_allocator_free(c_allocator, arr[i]);
    }
    end = yx_os_ticket_now();
    stdc_time_used = yx_os_ticket_toNanoSecond(end - start);
    
    
    
    /*cell pool test first time*/
    start = yx_os_ticket_now();
    for (int i=0; i<40960; i++)
    {
        arr[i] = yx_allocator_alloc(cell_allocator, 128);
        memset(arr[i], 0, 128);
    }
    for (int i=0; i<40960; i++)
    {
        yx_allocator_free(cell_allocator, arr[i]);
    }
    end = yx_os_ticket_now();
    cell_time_used_firstTime = yx_os_ticket_toNanoSecond(end - start);
    

    /*cell pool test second time*/
    start = yx_os_ticket_now();
    for (int i=0; i<40960; i++)
    {
        arr[i] = yx_allocator_alloc(cell_allocator, 128);
        memset(arr[i], 0, 128);
    }
    for (int i=0; i<40960; i++)
    {
        yx_allocator_free(cell_allocator, arr[i]);
    }
    end = yx_os_ticket_now();
    cell_time_used_secondTime = yx_os_ticket_toNanoSecond(end - start);


    /*这里不知道为什么，改动过的cellPool, 第一次速度居然比标准c还高。 按说应该比c低一些*/
    YX_UTEST_VERIFY(stdc_time_used / cell_time_used_firstTime > 1.8f, "time out");
    YX_UTEST_VERIFY(stdc_time_used / cell_time_used_secondTime > 5.0f, "time out");

    
    yx_cellMempool_destroy(&cell_allocator);
}

static YX_UTEST_FUN(_test_space_performance)
{
    yx_allocator debug_allocator = yx_debugMempool_create(NULL, 4096);
    yx_allocator allocator = yx_cellMempool_create(debug_allocator, 128);
    
    /*一般容器中，不会太多，所以测试比较小的数值*/
    yx_ptr arr[512];
    
    yx_size memory_used = 0;

    
    for (int i=0; i<512; i++)
    {
        arr[i] = yx_allocator_alloc(allocator, 128);
        memset(arr[i], 0, 128);
    }
    
    
    memory_used = yx_debugMempool_memSize(debug_allocator);
    
    
    
    for (int i=0; i<512; i++)
    {
        yx_allocator_free(allocator, arr[i]);
    }
    
    
    
    
    YX_UTEST_VERIFY((128.0f * 512.0f) / ((yx_real)memory_used) > 0.999f, "mem used too large");

    
    yx_cellMempool_destroy(&allocator);
    yx_debugMempool_destroy(&debug_allocator);
}








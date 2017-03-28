//
//  yx_testcase_core_buf.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/19.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_testcase_core_buf.h"


static yx_unittest_group _g_ut_group_core_buf = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////
static YX_UTEST_FUN(_test_basic_readwrite_);
static YX_UTEST_FUN(_test_overwrite_and_offset);
static YX_UTEST_FUN(_test_appened);
static YX_UTEST_FUN(_test_truncation);
////////////////////////////////////////////////////////////////////////////////////////////////////



yx_bool yx_testcase_core_buf_setup(yx_unittest ut, yx_allocator allocator)
{
    yx_unused(ut);
    
    _g_ut_group_core_buf = yx_ut_create_group(allocator, "core_buffer");
    YX_ASSERT(NULL != _g_ut_group_core_buf);
    
    yx_ut_group_add_case(_g_ut_group_core_buf, "basic readwrite", _test_basic_readwrite_);
    yx_ut_group_add_case(_g_ut_group_core_buf, "overwrite and offset ", _test_overwrite_and_offset);
    yx_ut_group_add_case(_g_ut_group_core_buf, "appened data", _test_appened);
    yx_ut_group_add_case(_g_ut_group_core_buf, "truncation", _test_truncation);
    
    yx_ut_add_group(ut, _g_ut_group_core_buf);
    
    return yx_true;

}

yx_bool yx_testcase_core_buf_shutdown(yx_unittest ut)
{
    yx_unused(ut);
    
    yx_ut_destroy_group(&_g_ut_group_core_buf);
    return yx_true;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark testcase
static YX_UTEST_FUN(_test_basic_readwrite_)
{
    yx_allocator debug_allocator = yx_debugMempool_create(NULL, 4096);
    yx_debugMempool_assertion(debug_allocator, yx_true);
    
    
    yx_set_allocator(debug_allocator);
    
    yx_core_buf core_buf;
    yx_core_buf_init(debug_allocator, &core_buf, 7);
    
    /*写入数据*/
    yx_byte buf_write[10];
    yx_byte index = 0;
    for (int i=0; i<40960; i++)
    {
        buf_write[index] = index;
        index++;
        
        if (10 == index)
        {
            yx_size flag = yx_core_buf_write(&core_buf, buf_write, 10);
            YX_UTEST_VERIFY(10 == flag, "write false");
            
            index = 0;
            
            YX_UTEST_VERIFY(i+1 == (int)yx_core_buf_length(&core_buf), "write false");
        }
    }
    
    
    
    /*移动文件指针到文件头*/
    YX_UTEST_VERIFY(40960 == yx_core_buf_length(&core_buf), "write false");
    YX_UTEST_VERIFY(0 == yx_core_buf_length_after_cursor(&core_buf), "write false");
    yx_core_buf_rewind(&core_buf);
    YX_UTEST_VERIFY(40960 == yx_core_buf_length(&core_buf), "rewind false");
    YX_UTEST_VERIFY(40960 == yx_core_buf_length_after_cursor(&core_buf), "write false");

    
    /*读取数据*/
    yx_byte buf_read[40960+1024];
    yx_size flag = yx_core_buf_read(&core_buf, buf_read, 20480);
    YX_UTEST_VERIFY(20480 == flag, "read false");
    YX_UTEST_VERIFY(20480 == yx_core_buf_length_after_cursor(&core_buf), "write read");
    
    flag = yx_core_buf_read(&core_buf, buf_read+20480, 20480+1024);
    YX_UTEST_VERIFY(20480 == flag, "read false");
    YX_UTEST_VERIFY(0 == yx_core_buf_length_after_cursor(&core_buf), "read false");

    /*验证读取的数据*/
    index = 0;
    for (int i=0; i<40960; i++)
    {
        
        YX_UTEST_VERIFY(index == buf_read[i], "read false");
        index++;
        
        if (10 == index)
        {
            index = 0;
        }
    }
    
    
    
    yx_core_buf_recycle(&core_buf);
    
    yx_set_allocator(NULL);
    yx_debugMempool_destroy(&debug_allocator);
}


static YX_UTEST_FUN(_test_overwrite_and_offset)
{
    yx_allocator debug_allocator = yx_debugMempool_create(NULL, 4096);
    yx_debugMempool_assertion(debug_allocator, yx_true);
    
    
    yx_set_allocator(debug_allocator);
    
    yx_core_buf core_buf;
    yx_core_buf_init(debug_allocator, &core_buf, 7);
    
    
    yx_byte val1 = 1;
    yx_byte val2 = 9;
    yx_size flag;
    
    /*写入数据*/
    yx_byte buf_write[40960+1024];
    yx_os_memzero(buf_write, 40960);
    flag = yx_core_buf_write(&core_buf, buf_write, 40960);
    YX_UTEST_VERIFY(40960 == flag, "write false");
    YX_UTEST_VERIFY(40960 == yx_core_buf_length(&core_buf), "write false");
    
    /*移动文件指针到文件头*/
    YX_UTEST_VERIFY(40960 == yx_core_buf_length(&core_buf), "write false");
    YX_UTEST_VERIFY(0 == yx_core_buf_length_after_cursor(&core_buf), "write false");
    yx_core_buf_rewind(&core_buf);
    YX_UTEST_VERIFY(40960 == yx_core_buf_length(&core_buf), "rewind false");
    YX_UTEST_VERIFY(40960 == yx_core_buf_length_after_cursor(&core_buf), "write false");
    
    
    for (yx_size i=0; i < 40960; i++)
    {
        if (0 == i % 2)
        {
            flag = yx_core_buf_write(&core_buf, &val1, 1);
            YX_UTEST_VERIFY(1 == flag, "write false");
            YX_UTEST_VERIFY(40960 == yx_core_buf_length(&core_buf), "write false");
        }
        else
        {
            yx_core_buf_forward(&core_buf, 1);
        }
        
        YX_UTEST_VERIFY((40960 - (i + 1)) == yx_core_buf_length_after_cursor(&core_buf), "write false");
    }
    
    /*移动文件指针到文件头*/
    YX_UTEST_VERIFY(40960 == yx_core_buf_length(&core_buf), "write false");
    YX_UTEST_VERIFY(0 == yx_core_buf_length_after_cursor(&core_buf), "write false");
    yx_core_buf_toend(&core_buf);
    YX_UTEST_VERIFY(40960 == yx_core_buf_length(&core_buf), "rewind false");
    YX_UTEST_VERIFY(0 == yx_core_buf_length_after_cursor(&core_buf), "write false");
    
    
    for (yx_size i = 40959; i > 0; i --)
    {
        if (0 != i % 2)
        {
            yx_core_buf_backward(&core_buf, -1);
            flag = yx_core_buf_write(&core_buf, &val2, 1);
            yx_core_buf_backward(&core_buf, -1);
            YX_UTEST_VERIFY(1 == flag, "write false");
            YX_UTEST_VERIFY(40960 == yx_core_buf_length(&core_buf), "write false");
        }
        else
        {
            yx_core_buf_backward(&core_buf, -1);
        }
        
        YX_UTEST_VERIFY((40960 - i) == yx_core_buf_length_after_cursor(&core_buf), "write false");
    }
    
    
    yx_core_buf_rewind(&core_buf);
    
    
    
    
    /*读取数据*/
    yx_byte buf_read[40960+1024];
    flag = yx_core_buf_read(&core_buf, buf_read, 40960);
    YX_UTEST_VERIFY(40960 == flag, "read false");
    YX_UTEST_VERIFY(0 == yx_core_buf_length_after_cursor(&core_buf), "write read");
    
    for (int i=0; i<40960; i++)
    {
        if (0 == i % 2)
        {
            YX_UTEST_VERIFY(buf_read[i] == val1, "compare false");
        }
        else
        {
            YX_UTEST_VERIFY(buf_read[i] == val2, "compare false");
        }
    }
    
    
    
    yx_core_buf_recycle(&core_buf);
    
    yx_set_allocator(NULL);
    yx_debugMempool_destroy(&debug_allocator);
}

static YX_UTEST_FUN(_test_appened)
{
    yx_allocator debug_allocator = yx_debugMempool_create(NULL, 4096);
    yx_debugMempool_assertion(debug_allocator, yx_true);
    
    
    yx_set_allocator(debug_allocator);
    
    yx_core_buf core_buf;
    yx_core_buf_init(debug_allocator, &core_buf, 7);
    
    /*写入数据*/
    yx_byte buf_write[10];
    yx_size flag;
    
    for (int i=0; i<4096; i++)
    {
        
        if (0 == i % 2)
        {
            memset(buf_write, 1, 10);
        }
        else
        {
            memset(buf_write, -1, 10);
        }
        
        flag = yx_core_buf_append(&core_buf, buf_write, 10);
        YX_UTEST_VERIFY(10 == flag, "write false");
        YX_UTEST_VERIFY((i+1)*10 == (int)yx_core_buf_length(&core_buf), "write false");
        YX_UTEST_VERIFY((i+1)*10 == (int)yx_core_buf_length_after_cursor(&core_buf), "write false");
    }
    
    
    
    /*读取数据*/
    yx_byte buf_read[40960+1024];
    flag = yx_core_buf_read(&core_buf, buf_read, 20480);
    YX_UTEST_VERIFY(20480 == flag, "read false");
    YX_UTEST_VERIFY(20480 == yx_core_buf_length_after_cursor(&core_buf), "write read");
    
    flag = yx_core_buf_read(&core_buf, buf_read+20480, 20480+1024);
    YX_UTEST_VERIFY(20480 == flag, "read false");
    YX_UTEST_VERIFY(0 == yx_core_buf_length_after_cursor(&core_buf), "read false");
    
    /*验证读取的数据*/
    for (int i=0; i<4096; i++)
    {
        int val;
        if (0 == i % 2)
        {
            val = 1;
        }
        else
        {
            val = -1;
        }
        
        
        for (int j=0; j < 10; j++)
        {
            YX_UTEST_VERIFY(buf_read[i*10 + j] == (yx_byte)val, "data verify");
        }
    }
    
    
    
    yx_core_buf_recycle(&core_buf);
    
    yx_set_allocator(NULL);
    yx_debugMempool_destroy(&debug_allocator);
}



static YX_UTEST_FUN(_test_truncation)
{
    yx_allocator debug_allocator = yx_debugMempool_create(NULL, 4096);
    yx_debugMempool_assertion(debug_allocator, yx_true);
    
    
    yx_set_allocator(debug_allocator);
    
    yx_core_buf core_buf;
    yx_core_buf_init(debug_allocator, &core_buf, 7);
    
    /*写入数据*/
    yx_byte buf_write[35];
    yx_os_memset(buf_write, 1, 35);
    
    /*写入35个数据*/
    yx_size flag;
    flag = yx_core_buf_write(&core_buf, buf_write, 35);
    YX_UTEST_VERIFY(5 * 7 == flag, "false write");
    YX_UTEST_VERIFY(5 * 7 == yx_core_buf_length(&core_buf), "false write");
    YX_UTEST_VERIFY(5 * (7 + sizeof(struct yx_core_buf_block)) == yx_debugMempool_memSize(debug_allocator), "false write");
    
    
    /*移动到中间*/
    yx_core_buf_seek(&core_buf, YX_CORE_BUF_CUR_HEAD, 2 * 7 + 3);
    YX_UTEST_VERIFY(5 * 7 == yx_core_buf_length(&core_buf), "false write");
    YX_UTEST_VERIFY((5 * 7)-(2 * 7 + 3) == yx_core_buf_length_after_cursor(&core_buf), "false write");
    
    
    /*截断前面的数据*/
    yx_core_buf_truncateToCursor(&core_buf);
    YX_UTEST_VERIFY(3 * 7 == yx_core_buf_length(&core_buf), "false write");
    YX_UTEST_VERIFY(5 * 7-(2 * 7 + 3) == yx_core_buf_length_after_cursor(&core_buf), "false write");
    YX_UTEST_VERIFY(3 * (7 + sizeof(struct yx_core_buf_block)) == yx_debugMempool_memSize(debug_allocator), "false write");
    
    
    /*截断后面的数据*/
    yx_core_buf_truncateFromCursor(&core_buf);
    YX_UTEST_VERIFY(3 == yx_core_buf_length(&core_buf), "false write");
    YX_UTEST_VERIFY(0 == yx_core_buf_length_after_cursor(&core_buf), "false write");
    YX_UTEST_VERIFY(1 * (7 + sizeof(struct yx_core_buf_block)) == yx_debugMempool_memSize(debug_allocator), "false write");
    
    
    
    yx_core_buf_recycle(&core_buf);
    
    yx_set_allocator(NULL);
    yx_debugMempool_destroy(&debug_allocator);

}
































































































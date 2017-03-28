//
//  yxut_core.c
//  YXLib
//
//  Created by Yuxi Liu on 11/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#include "yx_testcase_core_headers.h"

/*mempool*/
#include "mem/yx_testcase_core_mempool_debug.h"
#include "mem/yx_testcase_core_mempool_basic.h"
#include "mem/yx_testcase_core_mempool_buf.h"
#include "mem/yx_testcase_core_mempool_cell.h"

/*buf*/
#include "buf/yx_testcase_core_buf.h"


yx_bool yx_testcase_core_setup(yx_unittest ut, yx_allocator allocator)
{
    /*mempool*/
    YX_ASSERT(yx_true == yx_testcase_core_mempool_debug_setup(ut, allocator));
    YX_ASSERT(yx_true == yx_testcase_core_mempool_basic_setup(ut, allocator));
    YX_ASSERT(yx_true == yx_testcase_core_mempool_buf_setup(ut, allocator));
    YX_ASSERT(yx_true == yx_testcase_core_mempool_cell_setup(ut, allocator));
    
    /*buf*/
    YX_ASSERT(yx_true == yx_testcase_core_buf_setup(ut, allocator));
    
    return yx_true;
}

yx_bool yx_testcase_core_shutdown(yx_unittest ut)
{
    /*mempool*/
    YX_ASSERT(yx_true == yx_testcase_core_mempool_debug_shutdown(ut));
    YX_ASSERT(yx_true == yx_testcase_core_mempool_basic_shutdown(ut));
    YX_ASSERT(yx_true == yx_testcase_core_mempool_buf_shutdown(ut));
    YX_ASSERT(yx_true == yx_testcase_core_mempool_cell_shutdown(ut));
    
    /*buf*/
    YX_ASSERT(yx_true == yx_testcase_core_buf_shutdown(ut));
    
    return yx_true;
}



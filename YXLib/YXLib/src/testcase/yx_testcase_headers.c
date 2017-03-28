//
//  yx_testcase_headers.c
//  YXLib
//
//  Created by Yuxi Liu on 11/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//
#include "yx_testcase_headers.h"
#include "core/yx_testcase_core_headers.h"

yx_bool yx_testcase_setup(yx_unittest ut, yx_allocator allocator)
{
    YX_ASSERT(yx_true == yx_testcase_core_setup(ut, allocator));
    return yx_true;
}
yx_bool yx_testcase_shutdown(yx_unittest ut)
{
    YX_ASSERT(yx_true == yx_testcase_core_shutdown(ut));
    return yx_true;
}

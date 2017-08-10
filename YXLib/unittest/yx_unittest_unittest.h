//
//  yx_unittest_unittest.h
//  YXLib
//
//  Created by Yuxi Liu on 9/8/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_UNITTEST_UNITTEST_H_
#define _YX_UNITTEST_UNITTEST_H_

#include "yx_unittest_global.h"

typedef yx_handle yx_unittest;
typedef yx_handle yx_unittest_group;
typedef yx_handle yx_unittest_case;

#pragma mark test fun
typedef void (*yx_ut_testcase)(yx_unittest_case allocator);


#pragma mark group/case
/*yx_test*/
yx_unittest yx_ut_create(yx_allocator allocator);
void yx_ut_destroy(yx_unittest* test_handle_ptr);
void yx_ut_add_group(yx_unittest ut, yx_unittest_group group);

void yx_ut_set_repeat_times(yx_unittest ut, yx_uint times);
void yx_ut_run(yx_unittest ut);


/*yx_test_group*/
yx_unittest_group yx_ut_create_group(yx_allocator allocator, const yx_char* name);
void yx_ut_destroy_group(yx_unittest_group* test_group_ptr);

void yx_ut_group_add_case(yx_unittest_group group, const yx_char* case_name, yx_ut_testcase case_fun);
void yx_ut_group_set_repeat_times(yx_unittest_group group, yx_uint times);

/*case*/
void yx_ut_case_error(yx_unittest_case test_case, const char* file, yx_uint line, const yx_char* msg, yx_char* expr);
void yx_ut_case_set_time_limit(yx_unittest_case test_case, yx_os_time limit);


#define YX_UTEST_FUN(fun_name) \
void fun_name(yx_unittest_case __you_must_be_at_least_use_one_YX_UTTEST_XXX_fun__)

#define YX_UTEST_TIMELIMIT_DECLARE(time) \
yx_ut_case_set_time_limit(__you_must_be_at_least_use_one_YX_UTTEST_XXX_fun__, time)

#define YX_UTEST_VERIFY(expr, msg) \
if(!(expr)) {yx_ut_case_error(__you_must_be_at_least_use_one_YX_UTTEST_XXX_fun__, __FILE__, __LINE__, msg, #expr); return;}

/*utility*/
void yx_ut_printf(yx_unittest ut, yx_vaparam(char* format)) yx_vafun_declare;


#endif







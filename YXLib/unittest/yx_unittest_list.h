//
//  yx_unittest_utility.h
//  YXLib
//
//  Created by Yuxi Liu on 9/8/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_UNITTEST_UTILITY_H_
#define _YX_UNITTEST_UTILITY_H_

#include "yx_unittest_global.h"

typedef yx_handle _yx_ut_list;

typedef yx_bool (*yx_ut_callback_work)(_yx_ut_list list, yx_handle handle, yx_index index);
typedef void (*yx_ut_callback_destroy)(_yx_ut_list list, yx_handle handle, yx_index index);


_yx_ut_list _yx_ut_list_create(yx_allocator allocator);
void _yx_ut_list_destroy(_yx_ut_list* list_ptr, yx_ut_callback_destroy callback);

void _yx_ut_list_insert_front(_yx_ut_list list, yx_handle handle);
void _yx_ut_list_insert_back(_yx_ut_list list, yx_handle handle);

void _yx_ut_list_delete_node(_yx_ut_list list, yx_handle handle);

void _yx_ut_list_bindingData_set(_yx_ut_list list, yx_handle handle);
yx_handle _yx_ut_list_bindingData_get(_yx_ut_list list);

void _yx_ut_list_walk(_yx_ut_list list, yx_ut_callback_work callback);


#endif

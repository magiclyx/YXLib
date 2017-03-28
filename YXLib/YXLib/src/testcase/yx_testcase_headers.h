//
//  yx_testcase_headers.h
//  YXLib
//
//  Created by Yuxi Liu on 11/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef YXLib_yx_testcase_headers_h
#define YXLib_yx_testcase_headers_h

#include "yx_testcase_global.h"

yx_bool yx_testcase_setup(yx_unittest ut, yx_allocator allocator);
yx_bool yx_testcase_shutdown(yx_unittest ut);

#endif

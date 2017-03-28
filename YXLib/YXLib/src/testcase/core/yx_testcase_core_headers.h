//
//  Header.h
//  YXLib
//
//  Created by Yuxi Liu on 11/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_TESTCASE_CORE_H_
#define _YX_TESTCASE_CORE_H_ 

#include "yx_testcase_core_global.h"

yx_bool yx_testcase_core_setup(yx_unittest ut, yx_allocator allocator);
yx_bool yx_testcase_core_shutdown(yx_unittest ut);
#endif

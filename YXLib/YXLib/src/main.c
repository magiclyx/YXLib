//
//  main.c
//  YXLib
//
//  Created by Yuxi Liu on 9/3/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#include <stdio.h>
#include <assert.h>

#include "yx_core_headers.h"
#include "unittest/yx_unittest_headers.h"
#include "unittest/yx_unittest_unittest.h"

#include "testcase/core/yx_testcase_core_headers.h"


int utmain(int argc, const char * argv[])
{
    
    yx_unused(argc);
    yx_unused(argv);
    
    
    yx_unittest ut = yx_ut_create(NULL);
    
    yx_testcase_core_setup(ut, NULL);
    
    yx_ut_run(ut);
    
    yx_testcase_core_shutdown(ut);
    
    yx_ut_destroy(&ut);
    
    
    
    return 0;
}



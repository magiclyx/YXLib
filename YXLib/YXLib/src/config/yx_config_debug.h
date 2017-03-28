//
//  yx_config_debug.h
//  YXLib
//
//  Created by Yuxi Liu on 9/4/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CONFIG_DEBUG_H_
#define _YX_CONFIG_DEBUG_H_


#include "yx_config_global.h"


#pragma mark - debug switch
////////////////////////////////////////////////////////////////////////////////////////////////////
/*debug mode*/
////////////////////////////////////////////////////////////////////////////////////////////////////
#define YX_DEBUG 1




#pragma mark - assert
////////////////////////////////////////////////////////////////////////////////////////////////////
/*assert*/
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef YX_DEBUG
#  define YX_ASSERT_NOMAL 1    //normal assertion
//#  define YX_ASSERT_FORCE 1    //force assert
//#  define YX_ASSERT_LOG 1      //log the assert info
#endif


#pragma mark - verify
////////////////////////////////////////////////////////////////////////////////////////////////////
/*verify*/
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef YX_DEBUG

/* allocator */
//#define YX_BASICALLOC_DEBUG 1  //debug the basic debug

/* verify items */
//#define YX_COMMON_VERIFY  1     //for assert macro
#define YX_DATAMODE_VERIFY  1   //verify the datamode
#define YX_COMPILER_VERIFY  1   //verify the compiler
#define YX_PLATFORM_VERIFY  1   //verify the platform
#define YX_LANGUAGE_VERIFY 1   //verify the cplusplus macro
#define YX_ALIGHNMENT_VERIFY 1 //YX_ALIGHNMENT_VERIFY macro avaliable
#define YX_ALLOCATOR_VERIFY 1  //verify the allocator

#define YX_MEM_VERIFY 1        //verify the memory
//#define YX_SYSINFO_VERIFY 1    //verify the sysInfo

#endif






#endif

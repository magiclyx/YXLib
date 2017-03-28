//
//  yx_os_platform_macosx.h
//  YXLib
//
//  Created by Yuxi Liu on 9/4/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_PLATFORM_H_
#define _YX_MACOSX_PLATFORM_H_

#include "../yx_macosx_global.h"






#pragma mark - platform info
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *  lib support
 *
 *  YX_LIBSUPPORT_POSIX
 */

/**
 *  platform
 *
 *  YX_PLATFORM_NAME
 *  YX_PLATFORM_DESCRIPTION
 */


/**
 *  architectures
 *
 *  YX_ARCH_ENDINA_LITTLE   little-end
 *  YX_ARCH_ENDINA_BIG      big-end
 *
 *  YX_ARCH_X86
 *  YX_ARCH_X86_64
 *
 *  YX_ARCH_PPC
 *  YX_ARCH_PPC_64
 */

////////////////////////////////////////////////////////////////////////////////////////////////////

#if YX_PLATFORM_MACOSX

#  define YX_LIBSUPPORT_POSIX 1
#  define YX_PLATFORM_NAME "MacOSX"

#  if defined( __x86_64__ ) ||  defined( __x86_64 ) || defined( __amd64 )
#      define YX_ARCH_X86_64 1
#      define YX_ARCH_ENDINA_LITTLE 1
#      define YX_PLATFORM_DESCRIPTION "MacOSX x86-64"

#  elif defined( __i386__ ) || defined( __intel__ )
#      define YX_ARCH_X86 1
#      define YX_ARCH_ENDINA_LITTLE 1
#      define YX_PLATFORM_DESCRIPTION "MacOSX x86"

#  elif defined( __powerpc64__ ) || defined( __POWERPC64__ )
#      define YX_ARCH_PPC_64 1
#      define YX_ARCH_ENDINA_BIG 1
#      define YX_PLATFORM_DESCRIPTION "MacOSX PPC64"

#  elif defined( __powerpc__ ) || defined( __POWERPC__ )
#      define YX_ARCH_PPC 1
#      define YX_ARCH_ENDINA_BIG 1
#      define YX_PLATFORM_DESCRIPTION "MacOSX PPC"

#  else
#      error Unknown architecture
#  endif

#endif //YX_PLATFORM_MACOSX




#pragma mark - platform verify
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#if YX_PLATFORM_VERIFY

//--------------------------------
#  if !( defined( __APPLE__ ) && __APPLE__ )
#      error "not apple family system"
#  endif
//--------------------------------
#  ifndef __MACH__
#      error "not macosx system"
#  endif
//--------------------------------

#endif //YX_PLATFORM_VERIFY



#endif




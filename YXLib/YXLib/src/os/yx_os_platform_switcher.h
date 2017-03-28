//
//  yx_os_platform_switcher.h
//  YXLib
//
//  Created by Yuxi Liu on 9/4/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_OS_PLATFORM_SWITCHER_H_
#define _YX_OS_PLATFORM_SWITCHER_H_


#include "yx_os_global.h"


#ifdef YX_PLATFORM_MACOSX
#include "macosx/yx_macosx_headers.h"
#else
#error "unknown platform"
#endif


#endif

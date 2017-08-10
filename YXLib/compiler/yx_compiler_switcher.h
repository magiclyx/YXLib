//
//  yx_compiler_def.h
//  YXLib
//
//  Created by Yuxi Liu on 9/5/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_COMPILER_SWITCHER_H_
#define _YX_COMPILER_SWITCHER_H_

#if YX_COMPILER_CLANG //clang
#    include "clang/yx_clang_def.h"
#    include "clang/yx_CPlusPlus_def.h"
#else
#    error "unknown compiler"
#endif


#endif

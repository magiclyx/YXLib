//
//  yx_core_assert.h
//  YXLib
//
//  Created by Yuxi Liu on 9/5/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_ASSERT_H_
#define _YX_CORE_ASSERT_H_ 

#include "../yx_core_global.h"

/*ASSERT*/
#if YX_ASSERT_NOMAL

#  if YX_DEBUG
#      define YX_ASSERT(condition)   assert(condition)
#      define YX_ASSERT_MSG(condition, message) assert(condition)
#  else
#      define YX_ASSERT(condition)
#      define YX_ASSERT_MSG(condition, message)
#  endif

#elif YX_ASSERT_FORCE

#  define YX_ASSERT(condition) if(condition) yx_asm_break

#elif YX_ASSERT_LOG

//:~ TODO YX_ASSERT_LOG

#endif



/*STATIC ASSERT*/
#define __YX_STATIC_ASSERT_PRIVATE_JOIN_(A, B) __YX_STATIC_ASSERT_PRIVATE_JOIN_IMPL_(A, B)
#define __YX_STATIC_ASSERT_PRIVATE_JOIN_IMPL_(A, B) A ## B
#define YX_STATIC_ASSERT(condition) typedef char __YX_STATIC_ASSERT_PRIVATE_JOIN_(yx_static_assert_result, __LINE__) [condition? 1:-1]
#define YX_STATIC_ASSERT_MSG(condition, message) YX_STATIC_ASSERT(condition)

/*verify the aligment*/
#if YX_ALIGHNMENT_VERIFY
  #define YX_STATIC_ASSERT_ALIGMENT(struct)  YX_STATIC_ASSERT(0 == (sizeof(struct) % YX_ALIGNMENT))
#else
  #define YX_STATIC_ASSERT_ALIGMENT(struct)
#endif



#endif

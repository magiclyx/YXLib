//
//  yx_core_hash_string.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/26.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_hash_string.h"

#define YX_HASH_SHIFT_MULTIPLY 1


#undef HASH_SHIFT_MULTIPLY
#ifdef HASH_SHIFT_MULTIPLY
#  define HASH_MULTIPLY(dw)   (((dw) << 7) - (dw))
#else
#  define HASH_MULTIPLIER  101
#  define HASH_MULTIPLY(dw)   ((dw) * HASH_MULTIPLIER)
#endif



yx_uint32 _nativehash(const char* psz, yx_uint32 seed)
{
    
    // force compiler to use unsigned arithmetic
    
    const unsigned char* upsz = (const unsigned char*) psz;
    
    
    for ( ; *upsz; ++upsz)
        
        seed = HASH_MULTIPLY(seed) + *upsz;
    
    
    return seed;
    

}




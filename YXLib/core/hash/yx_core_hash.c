//
//  yx_core_hash.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/19.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_hash.h"



yx_core_hash yx_core_hash_scramble(yx_core_hash hash)
{
    
    // Here are 10 primes slightly greater than 10^9
    
    // 1000000007, 1000000009, 1000000021, 1000000033, 1000000087,
    
    // 1000000093, 1000000097, 1000000103, 1000000123, 1000000181.
    
    
    // large prime number, also used for scrambling
    const yx_uint32 RANDOM_PRIME = 1000000007UL;
    
    return yx_core_hash_scramble_limited(hash, RANDOM_PRIME);
}


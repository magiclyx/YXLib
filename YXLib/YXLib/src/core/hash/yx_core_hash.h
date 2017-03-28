//
//  yx_core_hash.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/19.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_HASH_H_
#define _YX_CORE_HASH_H_

#include "../yx_core_global.h"
#include "stringhash/yx_core_hash_string.h"



#if YX_DATAMODE_64
    typedef yx_uint64 yx_core_hash;
#   define YX_CORE_HASH_SIZE 64

#   define yx_core_hash_low(hash)     ((yx_uint32)(hash))
#   define yx_core_hash_hi(hash)      ((yx_uint32)(((yx_uint64)(hash) >> 32) & 0xFFFFFFFF))
#else
    typedef yx_uint32 yx_core_hash;
#   define YX_CORE_HASH_SIZE 32

#   define yx_core_hash_low(hash)     ((yx_uint32)(hash))
#   define yx_core_hash_hi(hash)      ((yx_uint32)(((yx_uint32)(hash) >> 16) & 0xFFFF))
#endif //YX_DATAMODE_64






/*
Produce a scrambled, randomish number in the range 0 to RANDOM_PRIME-1.
Applying this to the results of the other hash functions is likely to
produce a much better distribution, especially for the identity hash
functions such as Hash(char c), where records will tend to cluster at
the low end of the hashtable otherwise. LKRhash applies this internally
to all hash signatures for exactly this reason.
*/

yx_forceinline
yx_core_hash yx_core_hash_scramble_limited(yx_core_hash hash, yx_uint32 prime_limited)
{
    // default value for "scrambling constant"
    const yx_uint32 RANDOM_CONSTANT = 314159269UL;
    
    return (RANDOM_CONSTANT * hash) % prime_limited;
}

yx_core_hash yx_core_hash_scramble(yx_core_hash hash);


/*string*/
#if YX_DATAMODE_32
#  define yx_core_hash_string(str, len)  ((yx_core_hash)yx_core_hash_string32(str, len))
#else
#  define yx_core_hash_string(str, len)  ((yx_core_hash)yx_core_hash_string64(str, len))
#endif

/*char*/
#define yx_core_hash_char(char_val)    ((yx_core_hash)char_val)
#define yx_core_hash_uchar(uchar_val)  ((yx_core_hash)uchar_val)

/*short*/
#define yx_core_hash_short(short_val)  ((yx_core_hash)short_val)


/*int*/
#define yx_core_hash_int(int_val)      ((yx_core_hash)int_val)
#define yx_core_hash_uint(uint_val)    ((yx_core_hash)uint_val)

/*long*/
#if (4 == YX_LONG_SIZE)
    #define yx_core_hash_long(long_val)    ((yx_core_hash)long_val)
    #define yx_core_hash_ulong(ulong_val)  ((yx_core_hash)ulong_val)
#else
    #define yx_core_hash_long(long_val)    ( (yx_core_hash)(((yx_uint32)long_val) ^ ((yx_uint32)(long_val >> 32))) )
    #define yx_core_hash_ulong(ulong_val)  ( (yx_core_hash)(((yx_uint32)ulong_val) ^ ((yx_uint32)(ulong_val >> 32))) )
#endif



/*float & double*/
yx_forceinline
yx_core_hash yx_core_hash_float(yx_float32 float_val)
{
    union {
        yx_float32 f;
        yx_uint32 ui;
    }u;
    
    u.f = float_val;
    return (yx_core_hash)(u.ui);
}

yx_forceinline
yx_core_hash yx_core_hash_double(yx_double64 double_val)
{

#if YX_DATAMODE_32
    union {
        yx_double64 d;
        yx_uint32 ui[2];
    }u;
    
    u.d = double_val;
    return u.ui[0] * 101 + u.ui[1];
#else
    union {
        yx_double64 d;
        yx_uint64 ui;
    }u;
    
    u.d = double_val;
    return u.ui;
#endif
    
}

#define yx_core_hash_real(real_val) yx_core_hash_double(real_val)


#endif /* _YX_CORE_HASH_H_ */


















































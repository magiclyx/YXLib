//
//  yx_core_hash_cityhash.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/19.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//


#ifndef _YX_CORE_HASH_CITYHASH_H_
#define _YX_CORE_HASH_CITYHASH_H_


#include "../../yx_core_global.h"

typedef struct uint128{
    yx_uint64 first;
    yx_uint64 second;
}uint128;


#define yx_core_hash_cityhash128Low64(uint128_ptr)  ((uint128_ptr)->first)
#define yx_core_hash_cityhash128High64(uint128_ptr) ((uint128_ptr)->second)



// Hash function for a byte array.
yx_uint64 yx_core_cityhash64(const char *buf, size_t len);

// Hash function for a byte array.  For convenience, a 64-bit seed is also
// hashed into the result.
yx_uint64 yx_core_cityhash64WithSeed(const char *buf, size_t len, yx_uint64 seed);

// Hash function for a byte array.  For convenience, two seeds are also
// hashed into the result.
yx_uint64 yx_core_cityhash64WithSeeds(const char *buf, size_t len,
                           yx_uint64 seed0, yx_uint64 seed1);

// Hash function for a byte array.
void yx_core_cityhash128(uint128* result, const char *s, size_t len);

// Hash function for a byte array.  For convenience, a 128-bit seed is also
// hashed into the result.
void yx_core_cityhash128WithSeed(uint128* result, const char *s, size_t len, uint128* seed);

// Hash function for a byte array.  Most useful in 32-bit binaries.
yx_uint32 yx_core_cityhash32(const char *buf, size_t len);


#ifdef YX_OS_CPUCRC_SUPPORT

// Hash function for a byte array.
void yx_core_cityhash128_crc(uint128* result, const yx_char* s, yx_size len);

// Hash function for a byte array.  For convenience, a 128-bit seed is also
// hashed into the result.
void yx_core_cityhash128WithSeed_crc(uint128* result, const yx_char* s, size_t len, uint128* seed);

// Hash function for a byte array.  Sets result[0] ... result[3].
void yx_core_hash256_crc(const yx_char* s, yx_size len, yx_uint64* result);

#endif //YX_OS_CPUCRC_SUPPORT

#endif  // _YX_CORE_HASH_CITYHASH_H_





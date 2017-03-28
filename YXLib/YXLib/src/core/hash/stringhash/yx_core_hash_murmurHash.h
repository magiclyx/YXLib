//
//  yx_core_hash_murmurHash.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/23.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_HASH_MURMURHASH_H_
#define _YX_CORE_HASH_MURMURHASH_H_

#include "../../yx_core_global.h"

#define yx_core_murmurhash_default_seed 0xC7DAAB59

/*
 64版本来自murmurhash2
 32和128版本来自murmurhash3
 */

yx_uint64 yx_core_murmurhash64WithSeed ( const void * key, yx_size len, yx_uint seed);
yx_uint32 yx_core_murmurhash32WithSeed ( const void * key, yx_size len, yx_uint32 seed);
void yx_core_murmurhash128WithSeed ( const void * key, const yx_size len, const yx_uint32 seed, void * out );



#define yx_core_murmurhash32(key, len) yx_core_murmurhash32WithSeed(key, len, yx_core_murmurhash_default_seed)


#define yx_core_murmurhash64(key, len) yx_core_murmurhash64WithSeed(key, len, yx_core_murmurhash_default_seed)


#endif /* _YX_CORE_HASH_MURMURHASH_H_ */

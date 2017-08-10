//
//  yx_core_hash_spookyHash.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/23.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_HASH_SPOOKYHASH_H_
#define _YX_CORE_HASH_SPOOKYHASH_H_

#include "../../yx_core_global.h"


#define yx_core_spookyhash_default_seed 0xC7DAAB59


void yx_core_spookyhash128(
                           const void *message,
                           yx_size length,
                           yx_uint64 *hash1,
                           yx_uint64 *hash2);

yx_uint64 yx_core_spookyhash64(
                              const void *message,
                              yx_size length,
                              yx_uint64 seed);


yx_uint32 yx_core_spookyhash32(
                               const void *message,
                               yx_size length,
                               yx_uint32 seed);




#endif /* _YX_CORE_HASH_SPOOKYHASH_H_ */






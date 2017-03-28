//
//  yx_core_hash_string.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/26.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_HASH_STRING_H_
#define _YX_CORE_HASH_STRING_H_

#include "../../yx_core_global.h"

#include "yx_core_hash_murmurHash.h"
#include "yx_core_hash_cityhash.h"
#include "yx_core_hash_spookyHash.h"


/*
 这里是简单实现，效率很低的一个hash算法呢
 当没有配置hash函数时，提供一个效率很低的hash实现
 */
#define yx_core_rawhash_default_seed 0xC7DAAB59
yx_uint32 _nativehash(const char* psz, yx_uint32 seed);



#if YX_DATAMODE_32

#  if YX_HASH_STRING_MURMUR //murmurhash
     /* 32bit下总是使用 murmurhash */
#    define yx_core_hash_string32(str, len)  yx_core_murmurhash32(str, len)
#    define yx_core_hash_string64(str, len)  yx_core_murmurhash64(str, len)
#  else
#error  "you should always use murmurhash in 32bit"
#  endif

#else

#  if YX_HASH_STRING_MURMUR //murmurhash
#    define yx_core_hash_string32(str, len)  yx_core_murmurhash32(str, len)
#    define yx_core_hash_string64(str, len)  yx_core_murmurhash64(str, len)
#  elif YX_HASH_STRING_CITY //cityhash
#    define yx_core_hash_string32(str, len)  yx_core_cityhash32(str, len)
#    define yx_core_hash_string64(str, len)  yx_core_cityhash64(str, len)
#  elif YX_HASH_STRING_SPOOKY //spookyhash
#    define yx_core_hash_string32(str, len)  yx_core_spookyhash32(str, len, yx_core_spookyhash_default_seed)
#    define yx_core_hash_string64(str, len)  yx_core_spookyhash64(str, len, yx_core_spookyhash_default_seed)
#  else //_nativehash
#    define yx_core_hash_string32(str, len)  _nativehash(str, yx_core_rawhash_default_seed)
#    define yx_core_hash_string64(str, len)  (yx_uint64)_nativehash(str, yx_core_rawhash_default_seed)
#  endif


#endif



#endif /* _YX_CORE_HASH_STRING_H_ */

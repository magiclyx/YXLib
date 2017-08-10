//
//  yx_config_hash.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/20.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CONFIG_HASH_H_
#define _YX_CONFIG_HASH_H_


/*
 1. 32bit下, murmurhash 正常工作，cityhash 和 spookyhash 在32bit下表现不好
 2. 实际测试, 64bit下 debug模式，三种hash效率差不多
 2. 实际测试, 32bit下 release模式, cityhash > spookyhash > murmurhash。
 
 说明: cityhash 和 spookyhash, 很依赖编译器优化. 但不知道为什么spookyhash实现后，效率没有传言的那么高。
 因此配置上, 不要使用spookyhash. 测试环境为mac系统， clang编译器
 */

#define YX_HASH_STRING_MURMUR 0
#define YX_HASH_STRING_CITY 1
#define YX_HASH_STRING_SPOOKY 0


#endif /* _YX_CONFIG_HASH_H_ */

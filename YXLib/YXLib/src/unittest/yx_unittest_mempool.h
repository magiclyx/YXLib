//
//  yx_unittest_mempool.h
//  YXLib
//
//  Created by Yuxi Liu on 1/5/15.
//  Copyright (c) 2015 Yuxi Liu. All rights reserved.
//

#ifndef _YX_UNITTEST_MEMPOOL_H_
#define _YX_UNITTEST_MEMPOOL_H_ 


#include "yx_unittest_global.h"

//:~ TODO 解决完Debug问题，把这个东西从unittest中挪出去


//内存泄露检测
//越界检测
//分配内存不为NULL检测


//添加一个宏，统计这些数值的时候，是否对内存大小按照对齐

//最大内存使用
//平均内存使用


yx_allocator yx_unittest_mempool_create(yx_uint blockNum, yx_allocator allocator);
void yx_unittest_mempool_destroy(yx_allocator* allocator_ptr);

void* yx_unittest_mempool_alloc(yx_allocator allocator, yx_size size);
void yx_unittest_mempool_free(yx_allocator allocator, yx_ptr address);


#endif

//
//  yx_core_mempool_buf.h
//  YXLib
//
//  Created by LiuYuxi on 15/4/17.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_MEMPOOL_BUF_H_
#define _YX_CORE_MEMPOOL_BUF_H_

#include "../yx_core_global.h"


#define YX_BUFPOOL_STD_BUFFLEN 1024

/*
 The buff mempool is good at alloc a lot of small memory
 The buff mempool is also a kind of authorelease pool
 
 如果父内存池设定为cellPool或另一个bufPool, 它会自动向上查找，找到一个稳定呢内存池
 */



yx_allocator yx_bufMempool_create(yx_allocator allocator, yx_size size);
void yx_bufMempool_destroy(yx_allocator* allocator_ref);

/*
 使用当前 默认的分配器 和 标准大小 申请一块临时内存
 
 这个函数存在的原因是因为默认内存池，使用的是basicPool。basicPool不存在内存内存块分裂和合并的机制。
 过多的分配不同大小的内存块，会导致内存膨胀
 
 这个方法，多用于申请临时内存池，所以要求保证上层内存池可以正常的申请释放
 */
#define yx_bufMempool_create_std() yx_bufMempool_create(yx_get_allocator(), YX_BUFPOOL_STD_BUFFLEN)


void* yx_bufMempool_alloc(yx_allocator allocator, yx_size size);
void* yx_bufMempool_memalign(yx_allocator allocator, yx_size alignment, yx_size size);
void yx_bufMempool_free(yx_allocator allocator, yx_ptr ptr);
void yx_bufMempool_reset(yx_allocator allocator);

yx_int yx_bufMempool_blockNum(yx_allocator allocator);
yx_int yx_bufMempool_largeBlockUsedNum(yx_allocator allocator);
yx_int yx_bufMempool_largeblockFreeNum(yx_allocator allocator);

yx_size yx_bufMempool_remainSpace(yx_allocator allocator);
yx_size yx_bufMempool_waistSpace(yx_allocator allocator);
yx_size yx_bufMempool_structureSpace(yx_allocator allocator);





#endif /* defined(__YXLib__yx_core_mempool_buf__) */





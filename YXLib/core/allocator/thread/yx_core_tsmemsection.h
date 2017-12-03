//
//  yx_core_tsmemsection.h
//  YXCLib
//
//  Created by Yuxi Liu on 2017/12/2.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_TSMEMSECTION_H_
#define _YX_CORE_TSMEMSECTION_H_

#include "../../yx_core_global.h"


/*
 The buff mempool is good at alloc a lot of small memory
 The buff mempool is also a kind of authorelease pool
 
 如果父内存池设定为cellPool或另一个bufPool, 它会自动向上查找，找到一个稳定呢内存池
 */



yx_allocator yx_tsmemsection_create(yx_allocator allocator, yx_size size);
void yx_tsmemsection_destroy(yx_allocator* allocator_ref);

/*
 使用当前 默认的分配器 和 标准大小 申请一块临时内存
 
 这个函数存在的原因是因为默认内存池，使用的是basicPool。basicPool不存在内存内存块分裂和合并的机制。
 过多的分配不同大小的内存块，会导致内存膨胀
 
 这个方法，多用于申请临时内存池，所以要求保证上层内存池可以正常的申请释放
 */
#define yx_tsmemsection_create_std() yx_tsmemsection_create(yx_get_allocator(), YX_MEMSECTION_BUFFLEN)


void* yx_tsmemsection_alloc(yx_allocator allocator, yx_size size);
void* yx_tsmemsection_memalign(yx_allocator allocator, yx_size alignment, yx_size size);
void yx_tsmemsection_freelarge(yx_allocator allocator, yx_ptr ptr); //无特殊情况不要调用，回遍历大内存数组，影响效率
void yx_tsmemsection_reset(yx_allocator allocator);

/*get parent allocator*/
yx_allocator yx_tsmemsection_parentpool(yx_allocator allocator);


#endif /* _YX_CORE_TSMEMSECTION_H_ */

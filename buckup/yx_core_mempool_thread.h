//
//  yx_core_mempool_thread.h
//  YXLib
//
//  Created by LiuYuxi on 15/7/6.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_MEMPOOL_THREAD_H_
#define _YX_CORE_MEMPOOL_THREAD_H_

#include "../yx_core_global.h"

/*
 TODO  所有theadPool改名，否则会和线程池冲突
 */





typedef yx_handle yx_threadpool;


yx_threadpool yx_threadMempool_create(yx_allocator parentPool, yx_size size);
void yx_threadMempool_destroy(yx_threadpool* threadpool_ref);

yx_allocator yx_threadMempool_push(yx_threadpool threadPool);
void yx_threadMempool_pop(yx_threadpool threadPool);

void* yx_threadMempool_alloc(yx_allocator allocator, yx_size size);
void* yx_threadMempool_memalign(yx_allocator allocator, yx_size alignment, yx_size size);
void yx_threadMempool_free(yx_allocator allocator, yx_ptr ptr);



#endif /* defined(_YX_CORE_MEMPOOL_THREAD_H_) */

//
//  threadpool.h
//  TestSelectServer
//
//  Created by Yuxi Liu on 10/18/12.
//  Copyright (c) 2012 Yuxi Liu. All rights reserved.
//

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include "../yx_core_global.h"

#ifdef __cplusplus
extern "C"{
#endif


typedef void* yx_core_threadpool_ref;

//考虑global 的thread pool
    
//:~ 2 bugs here

//:~ TODO Need a flexible thread pool
//:~ 链接logger模块
//:~ 初始化了一万个thread, 这个要改
//:~ 没有destroy 方法
yx_core_threadpool_ref yx_core_threadpool_create(yx_allocator allocator, yx_int num_of_threads);
void yx_core_theadpool_destroy(yx_core_threadpool_ref* handleRef, yx_int blocking);
    
yx_int yx_core_thread_addTask(yx_core_threadpool_ref handle, void (*routine)(void*), void *data, yx_int blocking);

    
#ifdef __cplusplus
}
#endif

    
#endif /* THREADPOOL_H_ */

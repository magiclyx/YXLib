//
//  yx_core_mempool_cell.h
//  YXLib
//
//  Created by LiuYuxi on 15/6/29.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MEMPOOL_CELL_H_
#define _YX_MEMPOOL_CELL_H_

#include "../yx_core_global.h"

/*
 !!!! The cell mempool should not used as a parentpool
 
 用于应对特定大小内存反复申请释放，常用于bufPool
 */

yx_allocator yx_membucket_create(yx_allocator parentpool, yx_size cellSize);
void yx_membucket_destroy(yx_allocator* allocator_ref);

yx_ptr yx_membucket_alloc(yx_allocator allocator);
void yx_membucket_free(yx_allocator allocator, yx_ptr ptr);


/*get parent allocator*/
yx_allocator yx_membucket_parentpool(yx_allocator allocator);


#endif /* defined(__YXLib__yx_core_mempool_cell__) */

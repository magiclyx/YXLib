//
//  yx_mempool_basic.h
//  YXLib
//
//  Created by Yuxi Liu on 8/29/13.
//  Copyright (c) 2013 Yuxi Liu. All rights reserved.
//

#ifndef YXLib_yx_mempool_basic_h
#define YXLib_yx_mempool_basic_h

#include "../yx_core_global.h"

//max memory support (max value of the 64bit num)
#define YX_MEMPOOL_BASIC_LARGERMEMORY_BITSUPPORT 64 

//the max memory in slut is (YX_MEMPOOL_BASIC_SMALLMEMORY_SLOTNUM * YX_ALIGNMENT)
//if someone want a piece of memory larger than this value. it will alloc from the largememory slut.
#define YX_MEMPOOL_BASIC_SMALLMEMORY_SLOTNUM 1024


/**
 *  内存池缺陷(以64bit为例)
 *
 *  小内存，比较浪费空间，因为每块内存都会有一个内存头。
 *  还有因为我设计的原因， 这个内存池是16字节对齐的(实际上是2*YX_ALIGNMENT)。
 *  而且当你申请YX_ALIGNMENT奇数倍内存的时候，更能节省空间
 *  例如，你申请16字节。在计算大小的时候会算上头大小(头大小是8字节)，也就是24字节。然后计算对齐。
 *  因为要按照16字节对齐，所以实际申请的内存是32字节。
 *
 *  优点
 *  速度真的很快
 *  而且，今后如果支持多线程。每个槽都会有单独的自旋锁。很少冲突
 *  今后会制作内存映射图，可以在初始化的时候，按照不同应用，初始化内存池
 */



yx_allocator yx_basicMempool_create(yx_allocator allocator);
void yx_basicMempool_destroy(yx_allocator* allocator_ptr);

void* yx_basicMempool_alloc(yx_allocator allocator, yx_size size);
void* yx_basicMempool_memalign(yx_allocator allocator, yx_size alignment, yx_size size);
void yx_basicMempool_free(yx_allocator allocator, yx_ptr address);
#define yx_basicMempool_maxSlotSize() (YX_MEMPOOL_BASIC_SMALLMEMORY_SLOTNUM * YX_ALIGNMENT * 2)



////////////////////////////////////////////////////////////////////////////////////////////////////
//debug method
////////////////////////////////////////////////////////////////////////////////////////////////////
#if YX_ALLOCATOR_VERIFY
//this is a basic pool. so we must sure it's ok

#define YX_MEMPOOL_BASIC_DEBUG_LARGEMEM_START (YX_MEMPOOL_BASIC_SMALLMEMORY_SLOTNUM*YX_ALIGNMENT)

yx_bool isSmallSlutEmpty(yx_handle pool);
yx_bool isLargeSlutEmpty(yx_handle pool);

#endif //end of YX_ALLOCATOR_VERIFY


#endif




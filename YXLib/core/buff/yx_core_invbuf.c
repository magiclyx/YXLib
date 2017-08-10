//
//  yx_core_invbuf.c
//  YXLib
//
//  Created by yuxi on 16/8/2.
//  Copyright © 2016年 Yuxi Liu. All rights reserved.
//

#include "yx_core_invbuf.h"
#include "../debug/yx_core_assert.h"


void yx_core_infbuf_init(yx_core_infbuf_ref buf, yx_allocator buf_allocator, yx_size size){
    
    YX_ASSERT(NULL != buf);
    
    yx_ptr ptr = NULL;
    
    
    ptr = yx_allocator_alloc(buf_allocator, size);
    if (yx_unlikely( NULL == ptr ))
        goto errout;
    
    buf->buf_allocator = buf_allocator;
    yx_core_bufnode_init(&(buf->buf_node), ptr, size);
    buf->ref_count = 1;
    buf->refbuf = buf;
    
    
errout:
    
    if (NULL != ptr)
        yx_allocator_free(buf_allocator, ptr);
    
}

void yx_core_infbuf_initWith_infbuf(yx_core_infbuf_ref buf, yx_core_infbuf_ref original_buf){
    
    YX_ASSERT(NULL != buf);
    
    yx_core_infbuf_initWith_infbufExt(buf, original_buf, 0, (yx_uint)yx_core_bufnode_len(&(original_buf->buf_node)));
    
}

void yx_core_infbuf_initWith_infbufExt(yx_core_infbuf_ref buf, yx_core_infbuf_ref original_buf, yx_uint start, yx_uint len){
    
    YX_ASSERT(NULL != buf);
    YX_ASSERT ( start+len > yx_core_bufnode_len(&(original_buf->buf_node)) );

    
    //计数器，要从refbuf中计算
    yx_core_infbuf_ref refbuf = original_buf->refbuf;
    refbuf->ref_count++;
    
    
    //引用buf要从 original_buf 计算
    yx_core_bufnode_init(&(buf->buf_node), original_buf->buf_node.ptr + start, len);
    
    //其他初始化
    buf->buf_allocator = NULL;
    buf->ref_count = -1;
    buf->refbuf = refbuf;
    
}



void yx_core_infbuf_recycle(yx_core_infbuf_ref* buf_ptr){
    
    yx_core_infbuf_ref buf = (*buf_ptr)->refbuf;
    yx_core_infbuf_ref refbuf = (*buf_ptr)->refbuf;
    *buf_ptr = NULL;
    
    
    refbuf->ref_count--;
    if (0 == refbuf->ref_count)
        yx_allocator_free(refbuf->buf_allocator, refbuf->buf_node.ptr);
    
    yx_core_bufnode_recycle(&(buf->buf_node));
    buf->refbuf = NULL;
    buf->buf_allocator = NULL;
    buf->ref_count = -1;
}



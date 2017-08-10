//
//  yx_core_invbuf.h
//  YXLib
//
//  Created by yuxi on 16/8/2.
//  Copyright © 2016年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_INVBUF_H_
#define _YX_CORE_INVBUF_H_

#include "../yx_core_global.h"

typedef struct yx_core_inf_bufnode{
    yx_ptr ptr;
    yx_size len;
}yx_core_bufnode;


#define yx_core_bufnode_init(inf_bufnode, _ptr, _size) \
(inf_bufnode)->ptr = (_ptr); \
(inf_bufnode)->len = (_size)

#define yx_core_bufnode_recycle(inf_bufnode) \
(inf_bufnode)->ptr = NULL; \
(inf_bufnode)->len = YX_SIZE_INVALIDATE \


#define yx_core_bufnode_len(inf_bufnode) \
(inf_bufnode)->len

#define yx_core_bufnode_ptr(inf_bufnode) \
(inf_bufnode)->ptr



typedef struct yx_core_infbuf{
    
    yx_core_bufnode buf_node;
    
    struct yx_core_infbuf* refbuf; //指向原始的buf
    yx_allocator buf_allocator; //分配器
    yx_int ref_count; //引用计数
    
}yx_core_infbuf, *yx_core_infbuf_ref;


void yx_core_infbuf_init(yx_core_infbuf_ref buf, yx_allocator buf_allocator, yx_size size);
void yx_core_infbuf_initWith_infbuf(yx_core_infbuf_ref buf, yx_core_infbuf_ref original_buf);
void yx_core_infbuf_initWith_infbufExt(yx_core_infbuf_ref buf, yx_core_infbuf_ref original_buf, yx_uint start, yx_uint len);


void yx_core_infbuf_recycle(yx_core_infbuf_ref* buf_ptr);


#endif /* _YX_CORE_INVBUF_H_ */




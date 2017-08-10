//
//  yx_core_buf.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/18.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_BUF_H_
#define _YX_CORE_BUF_H_

#include "../yx_core_global.h"
#include "../container/yx_core_list.h"
#include "../debug/yx_core_assert.h"

typedef enum yx_core_buf_pos{
    YX_CORE_BUF_CUR_HEAD,
    YX_CORE_BUF_CUR_TAIL,
    YX_CORE_BUF_CUR_CURRENT,
}yx_core_buf_pos;



typedef struct yx_core_buf_block{
    struct yx_core_listnode node;
}yx_core_buf_block;

YX_STATIC_ASSERT_ALIGMENT(struct yx_core_buf_block);

typedef struct yx_core_buf_block* yx_core_buf_block_ref;




typedef struct yx_core_buf_cursor{
    yx_core_buf_block_ref pbufblock;
    yx_size buf_datasize; /*当前buf中的数据总长度*/
    yx_size offset;  /*在所有数据中的offset*/
    yx_byte* pstart;
    yx_byte* pend;
    yx_byte* plast;
}yx_core_buf_cursor;


typedef struct yx_core_buf_cursor* yx_core_buf_cursor_ref;





typedef struct yx_core_buf{
    
    struct yx_core_listnode list_head;
    
    yx_size buf_size;           /*这是每一块BUFF的大小, 真正的node_size是这个值＋节点大小*/
    
    yx_size data_size;          /*数据总长度户, 没有数据时和未初始化时，长度为0*/
    yx_size lastbuf_data_size;  /*最后一块buf中datasize*/
    
    
    yx_allocator allocator; /*这个allocator只是context用的内存池，使用init方法时，不会涉及到这个内存池，这个值为任一值*/
    yx_allocator buf_allocator; /*这个allocator 用于申请各个block*/
    
    
    /*关于cursor, 只有用到的时候才会初始化。因此如果你只是appened的话，它会是无效值, 在你read或移动指针的时候，会自动初始化*/
    yx_core_buf_cursor cursor;
    
}yx_core_buf;


typedef struct yx_core_buf* yx_core_buf_ref;


yx_core_buf_ref yx_core_buf_create(yx_allocator allocator, yx_allocator buf_allocator, yx_size size);
void yx_core_buf_destroy(yx_core_buf_ref* core_buf_ptr);

yx_bool yx_core_buf_init(yx_allocator buf_allocator, yx_core_buf_ref core_buf, yx_size buf_size);
void yx_core_buf_recycle(yx_core_buf_ref core_buf); //recyele后，buf中的数据会变成垃圾
void yx_core_buf_empty(yx_core_buf_ref core_buf); //clear后， buf中的数据会恢复到初始状态


#define yx_core_buf_length(core_buf)     ((core_buf)->data_size)
#define yx_core_buf_length_after_cursor(core_buf) ((yx_size)((core_buf)->data_size - (core_buf)->cursor.offset))
#define yx_core_buf_isEmpty(core_buf)    (0 == core_buf->data_size)


yx_size yx_core_buf_read(yx_core_buf_ref core_buf, yx_byte* cbuf, yx_size size);
yx_size yx_core_buf_write(yx_core_buf_ref core_buf, yx_byte* cbuf, yx_size size);
yx_size yx_core_buf_append(yx_core_buf_ref core_buf, yx_byte* cbuf, yx_size size);



void yx_core_buf_rewind(yx_core_buf_ref core_buf);
void yx_core_buf_toend(yx_core_buf_ref core_buf);

yx_bool yx_core_buf_forward(yx_core_buf_ref core_buf, yx_long offset);  //offset must larger than 0
yx_bool yx_core_buf_backward(yx_core_buf_ref core_buf, yx_long offset); //offset must smaller than 0

inline
yx_bool yx_core_buf_offset(yx_core_buf_ref core_buf, yx_long offset)
{
    if (offset > 0){
        return yx_core_buf_forward(core_buf, offset);
    }
    else if(offset < 0){
        return yx_core_buf_backward(core_buf, offset);
    }
    
    return yx_true;
}

yx_bool yx_core_buf_seek(yx_core_buf_ref core_buf, yx_core_buf_pos pos, yx_long offset);



yx_bool yx_core_buf_buckupcursor(yx_core_buf_ref core_buf, yx_core_buf_cursor_ref cursor);
yx_bool yx_core_buf_restorecursor(yx_core_buf_ref core_buf, yx_core_buf_cursor_ref cursor);


yx_bool yx_core_bufcursor_next_buf(yx_core_buf_ref core_buf);
yx_bool yx_core_bufcursor_prev_buf(yx_core_buf_ref core_buf);

void yx_core_buf_truncateToCursor(yx_core_buf_ref core_buf);
void yx_core_buf_truncateFromCursor(yx_core_buf_ref core_buf);


#if YX_DEBUG
void yx_core_buf_debug_print_as_string(yx_core_buf_ref core_buf);
#endif


#endif /* defined(_YX_CORE_BUF_H_) */













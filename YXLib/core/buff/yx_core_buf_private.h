//
//  yx_core_buf_private.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/24.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_BUF_PRIVATE_H_
#define _YX_CORE_BUF_PRIVATE_H_

/*cursor后数据的大小*/
#define __yx_core_buf_datasize_after_cursor(core_buf) ((core_buf)->data_size - (core_buf)->cursor.offset)

/*当前buff中，cursor前数据的大小*/
#define __yx_core_buf_datasize_inbuff_before_cursor(core_buf) \
( (yx_size)((core_buf)->cursor.plast - (core_buf)->cursor.pstart) )

/*当前buff中，cursor后数据的大小*/
#define __yx_core_buf_datasize_inbuff_after_cursor(core_buf) \
( (core_buf)->cursor.buf_datasize - __yx_core_buf_datasize_inbuff_before_cursor(core_buf) )

/*cursor后，当前buf剩余空间的大小, buf的数量必须不为0*/
#define __yx_core_buf_space_inbuff_after_cursor(core_buf) \
((core_buf)->buf_size - __yx_core_buf_datasize_inbuff_before_cursor(core_buf))

/*cursor后，剩余的所有空间*/
#define __yx_core_buf_total_space_after_cursor(core_buf) \
( __yx_core_buf_total_space(core_buf) - (core_buf)->cursor.offset )


/*当前的总空间， buf数量必须不为0*/
#define __yx_core_buf_total_space(core_buf) \
( (core_buf)->data_size + ((core_buf)->buf_size - (core_buf)->lastbuf_data_size) )


/*判断是否是最后一个buf块*/
#define __yx_core_bufcur_islastblock(core_buf) \
( yx_core_listnode_isLast(&((core_buf)->cursor.pbufblock->node), &(core_buf->list_head)) )

#define __yx_core_bufcur_isfirstblock(core_buf) \
( yx_core_listnode_isFirst(&((core_buf)->cursor.pbufblock->node), &(core_buf->list_head)) )


/*让cursor指向下一个buf块，必须保证当前不是最后一个buf块*/
#define __yx_core_buf_next(core_buf) \
do{ \
core_buf->cursor.offset += __yx_core_buf_datasize_inbuff_after_cursor(core_buf); \
core_buf->cursor.pbufblock = yx_core_listnode_nextNodeOwner(&(core_buf->cursor.pbufblock->node), struct yx_core_buf_block, node); \
core_buf->cursor.pstart = (yx_byte*)(core_buf->cursor.pbufblock) + sizeof(struct yx_core_buf_block); \
core_buf->cursor.plast = core_buf->cursor.pstart; \
core_buf->cursor.pend = core_buf->cursor.pstart + core_buf->buf_size; \
core_buf->cursor.buf_datasize = (__yx_core_bufcur_islastblock( (core_buf) ))? \
((core_buf)->lastbuf_data_size) : (core_buf)->buf_size; \
}while(0)

/*指向上一个块， 必须保证当前不是第一个块*/
#define __yx_core_buf_prev(core_buf) \
do{ \
core_buf->cursor.offset -= __yx_core_buf_datasize_inbuff_before_cursor(core_buf) - (core_buf)->buf_size; \
core_buf->cursor.pbufblock = yx_core_listnode_prevNodeOwner(&((core_buf)->cursor.pbufblock->node), struct yx_core_buf_block, node); \
core_buf->cursor.buf_datasize =  (core_buf)->buf_size; \
core_buf->cursor.pstart = (yx_byte*)(core_buf->cursor.pbufblock) + sizeof(struct yx_core_buf_block); \
core_buf->cursor.plast = (core_buf)->cursor.pstart; \
core_buf->cursor.pend = (core_buf)->cursor.plast + (core_buf)->buf_size; \
}while(0)



/*从core_buf中复制一个cursor*/
#define __yx_core_buf_copy_cursor(core_buf, sursor) \
do{ \
cursor->pbufblock = core_buf->cursor.pbufblock; \
cursor->buf_datasize = core_buf->cursor.buf_datasize; \
cursor->offset = core_buf->cursor.offset; \
cursor->pstart = core_buf->cursor.pstart; \
cursor->pend = core_buf->cursor.pend; \
cursor->plast = core_buf->cursor.plast; \
}while(0)

#define __yx_core_buf_set_cursor(core_buf, cursor) \
do{ \
core_buf->cursor.pbufblock = cursor->pbufblock; \
core_buf->cursor.buf_datasize = cursor->buf_datasize; \
core_buf->cursor.offset = cursor->offset; \
core_buf->cursor.pstart = cursor->pstart; \
core_buf->cursor.plast = cursor->plast; \
}while(0)


/*将游标返回头部, 必须保证至少有一个buf块*/
#define __yx_core_buf_rewind(core_buf) \
do{ \
yx_core_buf_block_ref first_block = yx_core_listnode_firstNodeOwner(&(core_buf->list_head), struct yx_core_buf_block, node); \
core_buf->cursor.pbufblock = first_block; \
core_buf->cursor.pstart = (yx_ptr)first_block + sizeof(struct yx_core_buf_block); \
core_buf->cursor.plast = core_buf->cursor.pstart; \
core_buf->cursor.pend = core_buf->cursor.pstart + core_buf->buf_size; \
core_buf->cursor.offset = 0; \
core_buf->cursor.buf_datasize = (__yx_core_bufcur_islastblock(core_buf)? (core_buf)->lastbuf_data_size : (core_buf)->buf_size); \
}while(0)


/*将游标返回尾部*/
#define __yx_core_buf_toend(core_buf) \
do{ \
core_buf->cursor.pbufblock = yx_core_listnode_lastNodeOwner(&(core_buf->list_head), struct yx_core_buf_block, node); \
core_buf->cursor.offset = core_buf->data_size; \
core_buf->cursor.buf_datasize = core_buf->lastbuf_data_size; \
core_buf->cursor.pstart = (yx_byte*)(core_buf->cursor.pbufblock) + sizeof(struct yx_core_buf_block); \
core_buf->cursor.plast = core_buf->cursor.pstart + core_buf->lastbuf_data_size; \
core_buf->cursor.pend = core_buf->cursor.pstart + core_buf->buf_size; \
}while(0)


#define __yx_core_buf_cursor_is_init(core_buf) (NULL != (core_buf)->cursor.pbufblock)


/*初始化游标， 必须保证至少有一个buf块*/
#define __yx_core_buf_init_cursor_if_need(core_buf) \
if ( ! __yx_core_buf_cursor_is_init(core_buf) ) { \
YX_ASSERT(NULL == core_buf->cursor.pstart); \
YX_ASSERT(NULL == core_buf->cursor.pend); \
YX_ASSERT(NULL == core_buf->cursor.plast); \
YX_ASSERT(0 == core_buf->cursor.offset); \
YX_ASSERT(0 == core_buf->cursor.offset); \
YX_ASSERT(0 == core_buf->cursor.buf_datasize); \
__yx_core_buf_rewind(core_buf); \
}

#endif

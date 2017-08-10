//
//  yx_core_buf.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/18.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_buf.h"
#include "yx_core_buf_private.h"




yx_core_buf_ref yx_core_buf_create(yx_allocator allocator, yx_allocator buf_allocator, yx_size size)
{
    
    if (NULL == allocator)
        allocator = yx_get_allocator();
    
    if (NULL == buf_allocator)
        allocator = yx_get_allocator();
    
    
    yx_core_buf_ref core_buf = yx_allocator_alloc(allocator, sizeof(struct yx_core_buf));
    if (yx_unlikely( NULL == core_buf ))
        goto errout;
    
    core_buf->allocator = allocator;
    
    if (yx_unlikely( ! yx_core_buf_init(buf_allocator, core_buf, size) ))
        goto errout;
    
    
    return core_buf;
    
errout:
    if (NULL != core_buf)
        yx_allocator_free(allocator, core_buf);
    
    return NULL;
}

void yx_core_buf_destroy(yx_core_buf_ref* core_buf_ptr)
{
    YX_ASSERT(NULL != core_buf_ptr);
    yx_core_buf_ref core_buf = *core_buf_ptr;
    *core_buf_ptr = NULL;
    
    
    yx_core_buf_recycle(core_buf);
    yx_allocator_free(core_buf->allocator, core_buf_ptr);
}



yx_bool yx_core_buf_init(yx_allocator buf_allocator, yx_core_buf_ref core_buf, yx_size buf_size)
{
    YX_ASSERT(NULL != core_buf);
    YX_ASSERT(NULL != buf_allocator);
    
    core_buf->buf_allocator = buf_allocator;
    
    core_buf->buf_size = buf_size;
//    core_buf->buf_num = 0;
    
    core_buf->data_size = 0;
    core_buf->lastbuf_data_size = 0;
    core_buf->cursor.offset = 0;
    
    
    core_buf->cursor.pbufblock = NULL;
    core_buf->cursor.buf_datasize = 0;
    core_buf->cursor.offset = 0;
    core_buf->cursor.pstart = NULL;
    core_buf->cursor.pend = NULL;
    core_buf->cursor.plast = NULL;
    
    
    yx_core_listnode_sentinelInit(&(core_buf->list_head));
    
    return yx_true;

}

void yx_core_buf_recycle(yx_core_buf_ref core_buf)
{
    yx_core_buf_block_ref buf, n;
    yx_core_listnode_ownerForEachSafe(buf, n, &(core_buf->list_head), node)
    {
        yx_core_listnode_del(&(buf->node));
        yx_allocator_free(core_buf->buf_allocator, buf);
    }

}

void yx_core_buf_empty(yx_core_buf_ref core_buf)
{
    yx_core_buf_block_ref buf, n;
    yx_core_listnode_ownerForEachSafe(buf, n, &(core_buf->list_head), node)
    {
        yx_core_listnode_del(&(buf->node));
        yx_allocator_free(core_buf->buf_allocator, buf);
    }

    core_buf->data_size = 0;
    core_buf->lastbuf_data_size = 0;
    core_buf->cursor.offset = 0;
    
    
    core_buf->cursor.pbufblock = NULL;
    core_buf->cursor.buf_datasize = 0;
    core_buf->cursor.offset = 0;
    core_buf->cursor.pstart = NULL;
    core_buf->cursor.pend = NULL;
    core_buf->cursor.plast = NULL;
}



yx_size yx_core_buf_read(yx_core_buf_ref core_buf, yx_byte* cbuf, yx_size size)
{
    YX_ASSERT(NULL != core_buf);
    YX_ASSERT(NULL != cbuf);
    YX_ASSERT(0 != size);
    
    
    if (yx_core_buf_isEmpty(core_buf))
        return 0;
    
    __yx_core_buf_init_cursor_if_need(core_buf);

    
    YX_ASSERT(0 != core_buf->lastbuf_data_size);
    
    
    /*cursor后面没有数据了，直接返回*/
    const yx_size max_size_can_read = __yx_core_buf_datasize_after_cursor(core_buf);
    if (0 == max_size_can_read)
        return 0;
    
    
    /*计算需最多能够读多少数据*/
    yx_size size_need_read = size;
    if (size_need_read > max_size_can_read){
        size_need_read = max_size_can_read;
    }
    
    
    /*
     当前buf剩余空间大小。如果没有空间，让cursor指向下一个块
     */
    yx_size datasize_remain_in_buf = __yx_core_buf_datasize_inbuff_after_cursor(core_buf);
    if (0 == datasize_remain_in_buf)
    {
        //因为指向最后一个位置了，但remainSize不为0, 所以后面肯定有块
        YX_ASSERT( !__yx_core_bufcur_islastblock(core_buf) );
        
        __yx_core_buf_next(core_buf);
        datasize_remain_in_buf = core_buf->cursor.buf_datasize;
        
        /*这个不应该是0了*/
        YX_ASSERT(0 != datasize_remain_in_buf);
    }
    
    
    
    yx_size size_read = 0;
    for (;;)
    {
        YX_ASSERT(0 != size_need_read);
        
        /*计算每次读取数据大小*/
        yx_size read_this_time = size_need_read;
        if (read_this_time > datasize_remain_in_buf)
            read_this_time = datasize_remain_in_buf;
        
        /*填充内存*/
//        cbuf = yx_os_memfill(cbuf, core_buf->cursor.plast, read_this_time);
        yx_os_memcpy(cbuf, core_buf->cursor.plast, read_this_time);
        size_read += read_this_time;
        
        /*更新本地内容*/
        cbuf += read_this_time;
        
        
        /*更新cursor中的内容*/
        core_buf->cursor.plast += read_this_time;
        core_buf->cursor.offset += read_this_time;
        
        size_need_read -= read_this_time;
        if (0 == size_need_read)
            break;
        
        /*readsize是根据总数据长度修正过，readsize不为0，因此后面必定还有块*/
        YX_ASSERT( !__yx_core_bufcur_islastblock(core_buf) );
       
        /*移动到下一块*/
        __yx_core_buf_next(core_buf);
        datasize_remain_in_buf = core_buf->cursor.buf_datasize;
    }
    
    return size_read;
}


yx_size yx_core_buf_write(yx_core_buf_ref core_buf, yx_byte* cbuf, yx_size size)
{
    YX_ASSERT(NULL != core_buf);
    YX_ASSERT(NULL != cbuf);
    YX_ASSERT(0 != size);
    
    const yx_byte* p = cbuf;
    
    yx_size size_writen = 0; //总共写入的数据
    yx_size space_in_buflist_after_cursor; //cursor后总共的空间
    yx_size space_in_buf_after_cursor; //当前buf中，cursor后的空间
    yx_bool should_init_cursor = yx_false; //是否需要初始化cursor
    
    
    if ( yx_core_listnode_isEmpty(&(core_buf->list_head)) ){
        space_in_buflist_after_cursor = 0;
        space_in_buf_after_cursor = 0;
        should_init_cursor = yx_true;
    }
    else{
        space_in_buflist_after_cursor = __yx_core_buf_total_space_after_cursor(core_buf);
        space_in_buf_after_cursor = __yx_core_buf_space_inbuff_after_cursor(core_buf);
    }
    
    while (0 != size)
    {
        /*如果总空间为0, 添加新的buff*/
        if (0 == space_in_buflist_after_cursor)
        {
            /*
             出现这种情况必须是下面两种情况
             1. buflist是空的
             2. 当前cursor所指的位置是最后的block且，已经用完所有空间
             */
            YX_ASSERT( yx_core_listnode_isEmpty(&(core_buf->list_head)) ||
                      (__yx_core_bufcur_islastblock(core_buf)  &&  0 == space_in_buf_after_cursor) );
            
            yx_core_buf_block_ref bufbloc = (yx_core_buf_block_ref)yx_allocator_alloc(core_buf->buf_allocator, sizeof(struct yx_core_buf_block) + core_buf->buf_size);
            if (yx_unlikely( NULL == bufbloc ))
                return size_writen;
            
            yx_core_listnode_insertTail(&(core_buf->list_head), &(bufbloc->node));
            
            
            /*更改最后一块数据大小*/
            core_buf->lastbuf_data_size = 0;
            
            
            /*更新后面空间的变量*/
            space_in_buflist_after_cursor += core_buf->buf_size;
            
            
            /*让cursor指向新块*/
            //core_buf->cursor.offset; //因为只是跳到下一个buff的开头，而上一个buff已经到末尾了，所以offset不会增加，不需要调用这句
            core_buf->cursor.pbufblock = bufbloc;
            core_buf->cursor.pstart = (yx_ptr)bufbloc + sizeof(struct yx_core_buf_block);
            core_buf->cursor.plast = core_buf->cursor.pstart;
            core_buf->cursor.pend = core_buf->cursor.pstart + core_buf->buf_size;
            core_buf->cursor.buf_datasize = 0;

            
            /*更改buf中空间的变量*/
            space_in_buf_after_cursor = core_buf->buf_size;
        }
        else if (0 == space_in_buf_after_cursor)
        {
            /*
             当前buf没有空间了，但后面的块还有空间
             出现这种情情况是在覆盖之前内容的时候
             */
            
            /*
             因为总空间不为0，但当期那块为0， 因此后面肯定还有其他块
             */
            YX_ASSERT(! __yx_core_bufcur_islastblock(core_buf) );
            
            __yx_core_buf_next(core_buf);
            
            space_in_buf_after_cursor = core_buf->buf_size;
        }
        
        /*计算这次需要写入数据的长度*/
        yx_size size_write_this_time = size;
        if (size_write_this_time > space_in_buf_after_cursor){
            size_write_this_time = space_in_buf_after_cursor;
        }
        
        /*写入数据*/
        yx_os_memcpy(core_buf->cursor.plast, p, size_write_this_time);
        
        /*更新所有距离相关的东西*/
        core_buf->cursor.offset += size_write_this_time;
        core_buf->cursor.plast += size_write_this_time;
        
        /*更新本地距离相关的东西*/
        p += size_write_this_time;
        space_in_buflist_after_cursor -= size_write_this_time;
        space_in_buf_after_cursor -= size_write_this_time;
        size -= size_write_this_time;
        size_writen += size_write_this_time;
        
        if (__yx_core_bufcur_islastblock(core_buf))
        {
            
            yx_size data_size_before_cursor = __yx_core_buf_datasize_inbuff_before_cursor(core_buf);
            
            /*
             如果不是最后一块buff， datasize 覆盖后，datasize不变。不用做任何事
             如果是最后一块buff， datasize 要取决于，是否小号过了之前的lastbuf_data_size
             */
            if (core_buf->cursor.buf_datasize < data_size_before_cursor){
                core_buf->cursor.buf_datasize = data_size_before_cursor;
            }
            
            /*
             如果lastbuf_data_size小于当前buf中cursor之前的数据，更新lastbuf_data_size
             */
            if (core_buf->lastbuf_data_size < data_size_before_cursor){
                core_buf->lastbuf_data_size = data_size_before_cursor;
            }
            
            
            /*
             如果总的data_size小于当前cursor的offset, 更新总的data_size
             */
            if (core_buf->data_size < core_buf->cursor.offset){
                core_buf->data_size = core_buf->cursor.offset;
            }
        }
        
    }
    
    return size_writen;
}

yx_size yx_core_buf_append(yx_core_buf_ref core_buf, yx_byte* cbuf, yx_size size)
{
    YX_ASSERT(NULL != core_buf);
    YX_ASSERT(NULL != cbuf);
    YX_ASSERT(0 != size);
    
    const yx_byte* p = cbuf;
    yx_byte* plast;
    yx_byte* pstart;
    
    yx_size size_writen = 0; //总共写入的数据
    yx_size space_in_buf; //最后buff缓冲中，cursor后的空间

    
    /*最后一个buff中，剩余的空间*/
    if ( yx_core_listnode_isEmpty(&(core_buf->list_head)) ){
        space_in_buf = 0;
#if YX_DEBUG
        plast = NULL;
        pstart = NULL;
#endif
    }
    else{
        space_in_buf = core_buf->buf_size - core_buf->lastbuf_data_size;
        yx_core_buf_block_ref lastblock = yx_core_listnode_lastNodeOwner(&(core_buf->list_head), struct yx_core_buf_block, node);
        pstart = (yx_byte*)lastblock + sizeof(struct yx_core_buf_block);
        plast = pstart + core_buf->lastbuf_data_size;
    }
    
    while (0 != size)
    {
        /*如果剩余空间为0， 添加新的buf*/
        if (0 == space_in_buf)
        {
            yx_core_buf_block_ref bufbloc = (yx_core_buf_block_ref)yx_allocator_alloc(core_buf->buf_allocator, sizeof(struct yx_core_buf_block) + core_buf->buf_size);
            if (yx_unlikely( NULL == bufbloc ))
                return size_writen;
            
            yx_core_listnode_insertTail(&(core_buf->list_head), &(bufbloc->node));
            
            pstart = (yx_byte*)bufbloc + sizeof(struct yx_core_buf_block);
            plast = pstart;
            space_in_buf = core_buf->buf_size;
            
            core_buf->lastbuf_data_size = 0;
        }
        
        /*计算这次需要拷入的数据长度*/
        yx_size size_write_this_time = size;
        if (size_write_this_time > space_in_buf)
            size_write_this_time = space_in_buf;
        
        
        /*考入数据，移动指针*/
        yx_os_memcpy(plast, p, size_write_this_time);
        plast += size_write_this_time;
        p += size_write_this_time;
        
        
        /*更新全局变量*/
        core_buf->data_size += size_write_this_time;
        core_buf->lastbuf_data_size = (yx_size)(plast - pstart);

        /*更细一些本地计数变量*/
        size_writen += size_write_this_time;
        space_in_buf -= size_write_this_time;
        size -= size_write_this_time;
    }

    return size_writen;
}




void yx_core_buf_rewind(yx_core_buf_ref core_buf)
{
    if (! yx_core_listnode_isEmpty(&(core_buf->list_head)) )
    {
        __yx_core_buf_rewind(core_buf);
    }
}

void yx_core_buf_toend(yx_core_buf_ref core_buf)
{
    if (! yx_core_listnode_isEmpty(&(core_buf->list_head)) )
    {
        __yx_core_buf_toend(core_buf);
    }

}

yx_bool yx_core_buf_forward(yx_core_buf_ref core_buf, yx_long offset)
{
    YX_ASSERT(offset > 0);
    
    if (yx_core_buf_isEmpty(core_buf))
        return yx_false;
    
    __yx_core_buf_init_cursor_if_need(core_buf);

    yx_size buf_data_size = __yx_core_buf_datasize_inbuff_after_cursor(core_buf);
    yx_size data_size = __yx_core_buf_datasize_after_cursor(core_buf);
    
    /*修正offset*/
    yx_size forward_size = (yx_size)offset;
    if (forward_size > data_size)
        forward_size = data_size;
    
    
    while (0 != forward_size)
    {
        if (0 == buf_data_size)
        {
            /*因为当前buf已经没有数据了，forward_size不为0， 因此后面必定还有buf块*/
            YX_ASSERT(! __yx_core_bufcur_islastblock(core_buf));
            
            __yx_core_buf_next(core_buf);
            buf_data_size = __yx_core_buf_datasize_inbuff_after_cursor(core_buf);
        }
        
        /*计算这次移动多少*/
        yx_size forward_this_time = forward_size;
        if (forward_this_time > buf_data_size)
            forward_this_time = buf_data_size;
        
        core_buf->cursor.plast += forward_this_time;
        core_buf->cursor.offset += forward_this_time;
        
        buf_data_size -= forward_this_time;
        forward_size -= forward_this_time;
    }
    
    return yx_true;
}

yx_bool yx_core_buf_backward(yx_core_buf_ref core_buf, yx_long offset)
{
    YX_ASSERT(offset < 0);
    
    __yx_core_buf_init_cursor_if_need(core_buf);
    
    yx_size buf_data_size = __yx_core_buf_datasize_inbuff_before_cursor(core_buf);
    yx_size data_size = core_buf->cursor.offset;

    /*修正offset*/
    yx_size backward_size = (yx_size)(-offset);
    if (backward_size > data_size)
        backward_size = data_size;
    
    
    while (0 != backward_size)
    {
        if (0 == buf_data_size)
        {
            /*因为当前buf前面已经没有数据了，forward_size不为0， 因此前面必定还有buf块*/
            YX_ASSERT(! __yx_core_bufcur_isfirstblock(core_buf));
            
            //core_buf->cursor.offset -= buf_data_size; //前面已经算过了，这里不用再算,其实什么都不用减去，因为本来就是0
            core_buf->cursor.pbufblock = yx_core_listnode_prevNodeOwner(&(core_buf->cursor.pbufblock->node), struct yx_core_buf_block, node);
            core_buf->cursor.buf_datasize =  core_buf->buf_size;
            core_buf->cursor.pstart = (yx_byte*)(core_buf->cursor.pbufblock) + sizeof(struct yx_core_buf_block);
            core_buf->cursor.plast = core_buf->cursor.pstart + core_buf->buf_size; //指向buf最后的位置
            core_buf->cursor.pend = core_buf->cursor.plast;

            buf_data_size = core_buf->buf_size;
        }
        
        /*计算这次移动多少*/
        yx_size backward_this_time = backward_size;
        if (backward_this_time > buf_data_size)
            backward_this_time = buf_data_size;
        
        
        core_buf->cursor.plast -= backward_this_time;
        core_buf->cursor.offset -= backward_this_time;
        
        buf_data_size -= backward_this_time;
        backward_size -= backward_this_time;
    }
    
    return yx_true;
}



yx_bool yx_core_buf_seek(yx_core_buf_ref core_buf, yx_core_buf_pos pos, yx_long offset)
{
    YX_ASSERT(NULL != core_buf);
    
    if (yx_core_buf_isEmpty(core_buf))
        return yx_false;
    
    
    
    /*
     计算当cursor位置
     */
    switch (pos) {
        case YX_CORE_BUF_CUR_HEAD:
        {
            __yx_core_buf_rewind(core_buf);
        }
            break;
        case YX_CORE_BUF_CUR_TAIL:
        {
            __yx_core_buf_toend(core_buf);
        }
            break;
        case YX_CORE_BUF_CUR_CURRENT:
            break;
            
        default:
            /*不支持的操作符*/
            YX_ASSERT(0);
    }

    
    if (offset > 0)
    {
        return yx_core_buf_forward(core_buf, offset);
    }
    else if(offset < 0)
    {
        return yx_core_buf_backward(core_buf, offset);
    }
    else
    {
        return yx_true;
    }
    
}

void yx_core_buf_truncateToCursor(yx_core_buf_ref core_buf)
{
    /*
     这个函数只能释放掉cursor指向数据块之前的块，而当前块cursor中的数据仍然有效
     */
    
    yx_core_buf_block_ref buf, n;
    yx_core_listnode_ownerForEachSafe(buf, n, &(core_buf->list_head), node)
    {
        if (buf == core_buf->cursor.pbufblock)
            break;
        
        yx_core_listnode_del(&(buf->node));
        yx_allocator_free(core_buf->buf_allocator, buf);
        
        /*因为前面都是完整的buf, 所以直接减去datasize*/
        core_buf->cursor.offset -= core_buf->buf_size;
        core_buf->data_size -= core_buf->buf_size;
    }
}

void yx_core_buf_truncateFromCursor(yx_core_buf_ref core_buf)
{
    /*
     这个函数释放掉cursor指向数据块后面的块，并删除当前块中cursor后面的数据 
     */
    
    if (__yx_core_bufcur_islastblock(core_buf))
        return;
    
    yx_core_buf_block_ref buf = yx_core_listnode_nextNodeOwner(&(core_buf->cursor.pbufblock->node), struct yx_core_buf_block, node);
    yx_core_buf_block_ref n;
    
    /*删掉cursor后面的块*/
    yx_core_listnode_ownerForEachFromCursorSafe(buf, n, &(core_buf->list_head), node)
    {
        yx_core_listnode_del(&(buf->node));
        yx_allocator_free(core_buf->buf_allocator, buf);
    }
    
    core_buf->cursor.buf_datasize = __yx_core_buf_datasize_inbuff_before_cursor(core_buf);
    core_buf->data_size = core_buf->cursor.offset; //cursor 的位置，就是所有数据的结尾
    core_buf->lastbuf_data_size = core_buf->cursor.buf_datasize;

}




yx_bool yx_core_buf_buckupcursor(yx_core_buf_ref core_buf, yx_core_buf_cursor_ref cursor)
{
    YX_ASSERT(NULL != core_buf);
    YX_ASSERT(NULL != cursor);
    
    if (yx_core_buf_isEmpty(core_buf))
        return yx_false;

    
    __yx_core_buf_copy_cursor(core_buf, cursor);
    
    return yx_true;
}

yx_bool yx_core_buf_restorecursor(yx_core_buf_ref core_buf, yx_core_buf_cursor_ref cursor)
{
    YX_ASSERT(NULL != core_buf);
    YX_ASSERT(NULL != cursor);
    YX_ASSERT(NULL != cursor->pbufblock);
    
    if (yx_core_buf_isEmpty(core_buf))
        return yx_false;

    __yx_core_buf_set_cursor(core_buf, cursor);

    return yx_true;
}





yx_bool yx_core_bufcursor_next_buf(yx_core_buf_ref core_buf)
{
    /*如果buflist是空的，返回失败*/
    if (yx_core_buf_isEmpty(core_buf))
        return yx_false;
    
    /*如果cursor没初始化，初始化到第一个块*/
    __yx_core_buf_init_cursor_if_need(core_buf);
    
    /*如果当前已经是最后一个buf块，返回失败*/
    if (__yx_core_bufcur_islastblock(core_buf))
        return yx_false;
    
    
    __yx_core_buf_next(core_buf);
    
    return yx_true;
}

yx_bool yx_core_bufcursor_prev_buf(yx_core_buf_ref core_buf)
{
    /*如果buflist是空的，返回失败*/
    if (yx_core_buf_isEmpty(core_buf))
        return yx_false;

    /*如果cursor没初始化，返回失败*/
    if (! __yx_core_buf_cursor_is_init(core_buf))
        return yx_false;
    
    /*如果当前已经是最后一个buf块，返回失败*/
    if (__yx_core_bufcur_isfirstblock(core_buf))
        return yx_false;
    
    
    __yx_core_buf_prev(core_buf);
    
    return yx_true;
}




#if YX_DEBUG
void yx_core_buf_debug_print_as_string(yx_core_buf_ref core_buf)
{
    /*
     这个函数只能释放掉cursor指向数据块之前的块，而当前块cursor中的数据仍然有效
     */
    
    
    __yx_core_buf_init_cursor_if_need(core_buf);
    
    
    yx_core_buf_block_ref buf;
    yx_core_listnode_ownerForEach(buf, &(core_buf->list_head), node)
    {
        for (yx_size i=0; i < core_buf->buf_size; i++)
        {
            yx_char* c = ((yx_char*)buf + sizeof(struct yx_core_buf_block) + i);
            yx_os_printf("%c", *c);
        }
        
        yx_os_printf("\n");
    }

}
#endif



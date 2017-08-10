//
//  yx_core_buf_pipe.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/24.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_buf_pipe.h"
#include "yx_core_buf_private.h"

yx_int yx_core_buf_dump_to_pipe(yx_core_buf_ref core_buf, yx_os_pipe_ref pipe, yx_size* size)
{
    YX_ASSERT(NULL != core_buf);
    YX_ASSERT(NULL != pipe);
    YX_ASSERT(0 != *size);
    
    
    if (yx_core_buf_isEmpty(core_buf))
        return 0;
    
    __yx_core_buf_init_cursor_if_need(core_buf);
    
    
    YX_ASSERT(0 != core_buf->lastbuf_data_size);
    
    yx_int err = 0;
    
    
    /*cursor后面没有数据了，直接返回*/
    const yx_size max_size_can_dump = __yx_core_buf_datasize_after_cursor(core_buf);
    if (0 == max_size_can_dump)
        return 0;
    
    
    /*计算需最多能够读多少数据*/
    yx_size size_need_dump = *size;
    if (size_need_dump > max_size_can_dump){
        size_need_dump = max_size_can_dump;
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
    
    
    
    yx_size size_dump = 0;
    for (;;)
    {
        YX_ASSERT(0 != size_need_dump);
        
        /*计算每次读取数据大小*/
        yx_size dump_this_time = size_need_dump;
        if (dump_this_time > datasize_remain_in_buf)
            dump_this_time = datasize_remain_in_buf;
        
        
        yx_size write_to_pipe;
        err = yx_os_pipe_write(pipe, core_buf->cursor.plast, dump_this_time, &write_to_pipe);
        /*填充内存*/
        size_dump += write_to_pipe;
        
        
        /*如果没有发生错误，肯定要读到数据*/
        YX_ASSERT(0 == err  &&  0 == write_to_pipe);
        YX_ASSERT(0 == err  &&  dump_this_time == write_to_pipe);
        
        
        /*更新cursor中的内容*/
        core_buf->cursor.plast += write_to_pipe;
        core_buf->cursor.offset += write_to_pipe;
        
        size_need_dump -= write_to_pipe;
        if (0 == size_need_dump  ||  0  !=  err)
            break;
        
        /*readsize是根据总数据长度修正过，readsize不为0，因此后面必定还有块*/
        YX_ASSERT( !__yx_core_bufcur_islastblock(core_buf) );
        
        /*移动到下一块*/
        __yx_core_buf_next(core_buf);
        datasize_remain_in_buf = core_buf->cursor.buf_datasize;
    }
    
    
    *size = size_dump;
    
    return err;
}


yx_int yx_core_buf_collect_from_pipe(yx_core_buf_ref core_buf, yx_os_pipe_ref pipe, yx_size* size)
{
    YX_ASSERT(NULL != core_buf);
    YX_ASSERT(NULL != pipe);
    YX_ASSERT(NULL != size);
    YX_ASSERT(0 != *size);
    
    yx_size size_need_collect = *size;
    
    yx_byte* plast;
    yx_byte* pstart;
    
    yx_size size_collected = 0; //总共写入的数据
    yx_size space_in_buf; //最后buff缓冲中，cursor后的空间
    
    yx_int err = 0;
    
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
    
    while (0 != size_need_collect)
    {
        /*如果剩余空间为0， 添加新的buf*/
        if (0 == space_in_buf)
        {
            yx_core_buf_block_ref bufbloc = (yx_core_buf_block_ref)yx_allocator_alloc(core_buf->buf_allocator, sizeof(struct yx_core_buf_block) + core_buf->buf_size);
            if (yx_unlikely( NULL == bufbloc )){
                err = errno = ENOMEM;
                break;
            }
            
            yx_core_listnode_insertTail(&(core_buf->list_head), &(bufbloc->node));
            
            pstart = (yx_byte*)bufbloc + sizeof(struct yx_core_buf_block);
            plast = pstart;
            space_in_buf = core_buf->buf_size;
            
            core_buf->lastbuf_data_size = 0;
        }
        
        /*计算这次需要拷入的数据长度*/
        yx_size size_write_this_time = size_need_collect;
        if (size_write_this_time > space_in_buf)
            size_write_this_time = space_in_buf;
        
        
        /*考入数据，移动指针*/
        yx_size read_from_pipe;
        err = yx_os_pipe_read(pipe, plast, size_write_this_time, &read_from_pipe);
        plast += read_from_pipe;
        
        YX_ASSERT(0 == err  &&  0 != read_from_pipe); //如果没发生错误，必定有读到数据
        YX_ASSERT(0 == err  &&  size_write_this_time == read_from_pipe); //没发生错误，要读的肯定等于读到的

        
        
        /*更新全局变量*/
        core_buf->data_size += read_from_pipe;
        core_buf->lastbuf_data_size = (yx_size)(plast - pstart);
        
        /*更细一些本地计数变量*/
        size_collected += read_from_pipe;
        space_in_buf -= read_from_pipe;
        size_need_collect -= read_from_pipe;
    }
    
    *size = size_collected;
    
    return err;
}


yx_int yx_core_buf_write_from_pipe(yx_core_buf_ref core_buf, yx_os_pipe_ref pipe, yx_size* size)
{
    YX_ASSERT(NULL != core_buf);
    YX_ASSERT(NULL != pipe);
    YX_ASSERT(NULL != size);
    YX_ASSERT(0 != *size);
    
    yx_size size_need_collect = *size;
    
    yx_int err = 0;
    //    yx_size size_writen = 0; //总共写入的数据
    yx_size size_collected = 0; //总共写入的数据
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
    
    while (0 != size_need_collect)
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
            if (yx_unlikely( NULL == bufbloc )){
                err = ENOMEM;
                break;
            }
            
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
        else if (0 == space_in_buf_after_cursor  &&  0 != err)
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
        yx_size size_collect_this_time = size_need_collect;
        if (size_collect_this_time > space_in_buf_after_cursor){
            size_collect_this_time = space_in_buf_after_cursor;
        }
        
        /*写入数据*/
        yx_size read_from_pipe;
        err = yx_os_pipe_read(pipe, core_buf->cursor.plast, size_collect_this_time, &read_from_pipe);
        YX_ASSERT(0 == err  &&  0 != read_from_pipe); //如果没发生错误，必定有读到数据
        YX_ASSERT(0 == err  &&  size_collect_this_time == read_from_pipe); //没发生错误，要读的肯定等于读到的
        
        /*更新所有距离相关的东西*/
        core_buf->cursor.offset += read_from_pipe;
        core_buf->cursor.plast += read_from_pipe;
        
        /*更新本地距离相关的东西*/
        space_in_buflist_after_cursor -= read_from_pipe;
        space_in_buf_after_cursor -= read_from_pipe;
        size_need_collect -= read_from_pipe;
        size_collected += read_from_pipe;
        
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
    
    *size = size_collected;
    
    return err;
}

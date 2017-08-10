//
//  yx_core_event_com.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/21.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_event_comm.h"
#include "yx_core_eventloop.h"
#include "../../communicator/yx_core_communicator.h"
#include "../../communicator/yx_core_communicator_eventdriver.h"

static void _yx_core_event_comm_didbind(yx_core_event_comm_ref event, yx_core_eventloop_ref eventloop);
static void _yx_core_event_comm_willunbind(yx_core_event_comm_ref event, yx_core_eventloop_ref eventloop);
static void _yx_core_event_comm_handle(yx_core_event_comm_ref event, yx_os_event_info_ref info);
static yx_bool _yx_core_event_comm_regist(yx_core_event_comm_ref event, yx_core_eventloop_ref eventloop);



yx_bool yx_core_event_comm_init( yx_allocator allocator,
                                 yx_allocator buf_allocator,    /*缓冲区内存分配器*/
                                 yx_core_event_comm_ref event,
                                 yx_core_commgenerator_ref gen,  /*通信类型*/
                                 yx_value data                 /*用户数据*/
                                )
{
    YX_CORE_EVENT_SUPERINIT(event, allocator, yx_core_event_type_communicator, data, _yx_core_event_comm_handle);
    
    /*初始化communicator*/
    yx_bool hasInit = yx_core_communicator_init(event->ce_allocator, &(event->communicator), gen);
    if (yx_unlikely( ! hasInit ))
        return yx_false;
    
    /*初始化buff*/
    if(yx_unlikely(! yx_core_buf_init(buf_allocator, &(event->read_buf), 0) ))
        goto errout;
    if(yx_unlikely(! yx_core_buf_init(buf_allocator, &(event->write_buf), 0) ))
        goto errout;
    
    
    /*更改绑定和非绑定通知函数*/
    yx_core_event_setbindnotif(event, _yx_core_event_comm_didbind);
    yx_core_event_setbindnotif(event, _yx_core_event_comm_willunbind);
    
    /*设置注册函数的行为*/
    yx_core_event_setregisthandle(event, _yx_core_event_comm_regist);
    
    /*设置其他标记*/
    event->regist_read = yx_false;
    event->regist_read = yx_false;


    
    
    
    return yx_true;
    
errout:
    if(hasInit)
        yx_core_communicator_recycle(&(event->communicator));
    
    
    return yx_false;
}


yx_bool yx_core_event_comm_setread(yx_core_event_comm_ref event, yx_bool wantRead)
{
    YX_ASSERT(yx_core_event_hasbind(event)); //没有绑定
    YX_ASSERT(! event->regist_read); //为什么会发生重复注册？
    
    if (event->regist_read)
        return yx_true;

    yx_bool success;
    
    if (wantRead)
    {
        success = yx_core_comeventdriver_register(&(event->communicator),
                                        &(event->ce_runloop->eventdriver),
                                        yx_core_comevent_read,
                                        event);
    }
    else
    {
        success = yx_core_comeventdriver_unregister(&(event->communicator),
                                          &(event->ce_runloop->eventdriver),
                                          yx_core_comevent_read);
    }
    
    
    if (yx_likely( success ))
        event->regist_read = (wantRead? 1 : 0);
    
    
    return yx_true;
}

yx_bool yx_core_event_comm_setwrite(yx_core_event_comm_ref event, yx_bool wantWrite)
{
    YX_ASSERT(yx_core_event_hasbind(event)); //没有绑定
    YX_ASSERT(! event->regist_write ); //为什么会发生重复注册？
    
    if (event->regist_write)
        return yx_true;

    yx_bool success;
    
    if (wantWrite) {
        success = yx_core_comeventdriver_register(&(event->communicator),
                                        &(event->ce_runloop->eventdriver),
                                        yx_core_comevent_write,
                                        event);
    }
    else {
        success = yx_core_comeventdriver_unregister(&(event->communicator),
                                          &(event->ce_runloop->eventdriver),
                                          yx_core_comevent_write);
    }
    
    if (yx_likely( success ))
        event->regist_write = (wantWrite? 1 : 0);

    
    return yx_true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

static void _yx_core_event_comm_didbind(yx_core_event_comm_ref event, yx_core_eventloop_ref eventloop)
{
    YX_CORE_EVENT_SUPER_DIDBIND_NOTIF(event, eventloop);
}

static void _yx_core_event_comm_willunbind(yx_core_event_comm_ref event, yx_core_eventloop_ref eventloop)
{
    YX_CORE_EVENT_SUPER_WILLUNBIND_NOTIF(event, eventloop);
    
    
    /* 注销之前所有的事件 */
    
    if (event->regist_read)
        yx_core_event_comm_setread(event, yx_false);
    
    if (event->regist_write)
        yx_core_event_comm_setwrite(event, yx_false);
    
}

static yx_bool _yx_core_event_comm_regist(yx_core_event_comm_ref event, yx_core_eventloop_ref eventloop)
{
    YX_CORE_EVENT_SUPERREGIST(event, eventloop);
    
    return yx_core_event_comm_setread(event, yx_true);
}

yx_bool yx_core_event_comm_senddata(yx_core_event_comm_ref event, yx_byte* buff, yx_size size)
{
    /*向buf中添加数据*/
    yx_size writesize = yx_core_buf_append(&(event->write_buf), buff, size);
    if (yx_unlikely( writesize != size ))
    {
        
        /*如果发生错误，删除掉添加一半的数据*/
        
        yx_core_buf_cursor buckup_cursor;
        
        yx_core_buf_buckupcursor(&(event->write_buf), &buckup_cursor); //备份当前cursor
        yx_core_buf_seek(&(event->write_buf), YX_CORE_BUF_CUR_TAIL, -((yx_long)writesize)); //移动到添加数据之前的位置
        yx_core_buf_truncateFromCursor(&(event->write_buf)); //从cursor位置截断之后的数据
        yx_core_buf_restorecursor(&(event->write_buf), &buckup_cursor); //回复之前的cursor
        
        return yx_false;
    }
    
    /*设置可读标记, 如果当前已经为可读，这个函数会直接返回*/
    return yx_core_event_comm_setwrite(event, yx_true);
}


static void _yx_core_event_comm_handle(yx_core_event_comm_ref event, yx_os_event_info_ref info)
{
//    YX_ASSERT(NULL != info->)
//yx_core_event_ref
    
    YX_ASSERT(NULL != event);
    YX_ASSERT(NULL != info);
    
    switch (info->type)
    {
        case EVFILT_READ:
        {
            yx_size size = yx_core_communicator_sizeCanRead_withEventDriver(&(event->communicator), info);
            if (0 == size  ||  YX_SIZE_INVALIDATE == size)
            {
                /*
                 如果当前通讯模块不能获得可可读缓冲区大小，或者缓冲区大小没有限制，那么读取缓冲区直到不能读为止
                 */
                size = YX_MAX_ULONG;
            }
            yx_core_communicator_read_to_corebuf(&(event->communicator), &(event->read_buf), &size);
            //:~TODO 失败咋办
        }
            break;
        case EVFILT_WRITE:
        {
            /*取当前buf中数据，通信接口可写数据的最小值*/
            yx_size size = yx_core_communicator_sizeCanWrite_withEventDriver(&(event->communicator), info);
            if (0 == size  ||  YX_SIZE_INVALIDATE == size)
            {
                /*
                 如果当前通讯模块不能获得可写缓冲区大小，或者缓冲区大小没有限制，那么直接取buf中剩余数据大小
                 */
                size = yx_core_buf_length_after_cursor(&(event->read_buf));
            }
            else
            {
                /*
                 写入buf中剩余数据大小 和 可写入大小 中小的那个
                 */
                size = YX_MIN( size, yx_core_buf_length_after_cursor(&(event->read_buf)) );
            }
                
            yx_core_communicator_write_from_corebuf(&(event->communicator), &(event->read_buf), &size);
            //:~TODO 失败咋办
        }
            break;
        default:
            assert(0);
            break;
    }
}




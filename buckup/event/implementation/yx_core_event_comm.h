//
//  yx_core_event_com.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/21.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _yx_core_event_com_H_
#define _yx_core_event_com_H_

#include "../yx_core_event.h"

#include "../../communicator/yx_core_communicator.h"
#include "../../buff/yx_core_buf.h"

typedef struct yx_core_event_comm* yx_core_event_comm_ref;


/*通信*/
typedef struct yx_core_event_comm{
    YX_COREEVENT_STRUCT_DECLARE;
    
    yx_core_communicator communicator; /*通讯组件*/
    
    yx_core_buf read_buf;  /*读缓冲区*/
    yx_core_buf write_buf; /*写缓冲区*/
    
    yx_os_event_info_ref read_info; /*读信息*/
    yx_os_event_info_ref write_info; /*写信息*/
    
    yx_uint regist_read:1;  /*是否注册了读事件*/
    yx_uint regist_write:1; /*是否注册了写事件*/
    
    void (*callback_receive_data)(yx_core_event_comm_ref event);
    void (*callback_send_data)(yx_core_event_comm_ref event);
}yx_core_event_comm;




/*设置event为communicator, 之前的信息将被清空*/
yx_bool yx_core_event_comm_init( yx_allocator allocator,
                                 yx_allocator buf_allocator,    /*缓冲区内存分配器*/
                                 yx_core_event_comm_ref event,
                                 yx_core_commgenerator_ref gen,  /*通信产生器*/
                                 yx_value data                 /*用户数据*/
                                );


yx_bool yx_core_event_comm_setread(yx_core_event_comm_ref event, yx_bool wantRead);
yx_bool yx_core_event_comm_setwrite(yx_core_event_comm_ref event, yx_bool wangWrite);

yx_bool yx_core_event_comm_senddata(yx_core_event_comm_ref event, yx_byte* buff, yx_size size);

#endif /* defined(_yx_core_event_com_H_) */








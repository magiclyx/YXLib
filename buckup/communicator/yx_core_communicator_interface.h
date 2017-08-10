//
//  yx_core_communicator_opt_fun.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/21.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_COMMUNICATOR_INTERFACE_H_
#define _YX_CORE_COMMUNICATOR_INTERFACE_H_

#include "../yx_core_global.h"
#include "../buff/yx_core_buf.h"


/*
 这个文件定义了communicator的通用接口
 所有communicator的实现都必须实现这个接口，并提供一个yx_core_commgenerator
*/

typedef struct yx_core_communicator*  yx_core_communicator_ref;
typedef enum yx_core_comeventdirver_opt yx_core_comeventdirver_opt;



typedef struct yx_core_commaddress{
    yx_value __reserve;
}yx_core_commaddress;


typedef struct yx_core_commoperation{
    
    /*init & recycle*/
    yx_bool (*init)(yx_allocator allocator, yx_core_communicator_ref communicator);
    void (*recycle)(yx_core_communicator_ref communicator);

    
    /*readwrite*/
    yx_bool (*read)(yx_core_communicator_ref communicator, yx_byte *buf, yx_size bufSize, yx_size *bytesRead);
    yx_bool (*write)(yx_core_communicator_ref communicator, yx_byte *buf, yx_size bufSize, yx_size *bytesWritten);
    
    yx_bool (*corebuf_read)(yx_core_communicator_ref communicator, yx_core_buf_ref core_buf, yx_size* size);
    yx_bool (*corebuf_write)(yx_core_communicator_ref communicator, yx_core_buf_ref core_buf, yx_size* size);
    
    /*event driver -- 用于向eventdriver中注册这个communicator*/
    yx_bool (*regist_event)(yx_core_communicator_ref communicator,
                            yx_os_eventdriver_ref eventdirver,
                            yx_core_comeventdirver_opt opt,
                            yx_value userdata);

    yx_bool (*unregist_event)(yx_core_communicator_ref communicator,
                              yx_os_eventdriver_ref eventdirver,
                              yx_core_comeventdirver_opt opt);
    
    /*
     event -- 用于根据eventdriver被触发是的参数eventinfo来计算有多少数据可读写
     //返回 YX_SIZE_INVALIDATE 说明无法获得
     //返回 0 说明可读写是无限制的
     */
    yx_size (*size_can_read_with_eventinfo)(yx_core_communicator_ref communicator,
                                            yx_os_event_info_ref eventinfo);
    yx_size (*size_can_write_with_eventinfo)(yx_core_communicator_ref communicator,
                                             yx_os_event_info_ref eventinfo);
    
}yx_core_commoperation;

typedef struct yx_core_commoperation* yx_core_commoperation_ref;



typedef struct yx_core_commgenerator
{
    yx_core_commaddress address;
    void (*setup_operator)(yx_core_communicator_ref communicator, struct yx_core_commgenerator* generator);
}yx_core_commgenerator;

typedef struct yx_core_commgenerator* yx_core_commgenerator_ref;





#endif /* defined(_YX_CORE_COMMUNICATOR_INTERFACE_H_) */





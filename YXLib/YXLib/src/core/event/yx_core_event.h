//
//  yx_core_event.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/13.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_EVENT_H_
#define _YX_CORE_EVENT_H_

#include "../yx_core_global.h"

#include "../container/yx_core_listnode.h"
#include "yx_core_event_type.h"


//declare
typedef struct yx_core_eventloop* yx_core_eventloop_ref;
typedef struct yx_core_event* yx_core_event_ref;


#define YX_COREEVENT_STRUCT_DECLARE yx_core_event __self__event__


/*event handl*/
typedef void (*yx_core_event_handle)(yx_core_event_ref event, yx_os_event_info_ref info);




/*
 不要直接使用yx_core_event!!!!
 */
typedef struct yx_core_event{
    yx_core_event_type type;                /*id*/
    yx_value data;                          /*data*/
    yx_allocator allocator;                 /*每个event可以使用不同的内存分配器*/
    
    struct yx_core_listnode node;           /*在event中的链表节点*/
    yx_core_eventloop_ref runloop;          /*绑定的runloop*/
    
    yx_core_event_handle event_handle;     /*事件处理函数*/
}yx_core_event;


typedef yx_core_event* yx_core_event_ref;


/* get/setter property */

//user data
#define yx_core_event_setData(event, data)  (((yx_core_event_ref)event)->data = data)
#define yx_core_event_data(event)           (((yx_core_event_ref)event)->data)

/*type*/
#define yx_core_event_setType(event, type)      ((event)->type = type)
#define yx_core_event_type(event)               ((event)->type)

/*listnode*/
#define yx_core_event_listnode(event)           ((event)->node)



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 讨论: 之前，这里面分为bind 和 regist 两步。目的是为了runloop管理所有的event, 即使当前不起作用的event.
    这个真的有必要么？ 现在不确定。 因此在重构之后去掉了.
 */

yx_bool yx_core_event_init(yx_core_event_ref event,
                           yx_allocator allocator,
                           yx_core_event_type type,
                           yx_value data,
                           yx_core_event_handle event_handle);




#endif /* defined(_YX_CORE_EVENT_H_) */







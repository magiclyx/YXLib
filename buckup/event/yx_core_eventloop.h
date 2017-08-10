//
//  yx_core_event.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/13.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_EVENTLOOP_H_
#define _YX_CORE_EVENTLOOP_H_

#include "../yx_core_global.h"
#include "../container/yx_core_listnode.h"
#include "yx_core_event.h"


/*
 警告， kqueue是不会被子进程继承的
 */


typedef struct yx_core_eventloop{
    yx_os_eventdirver eventdriver;
    yx_allocator allocator;
//    yx_os_critime_nsec timeout;
    
    struct yx_core_listnode list_head;
    
}yx_core_eventloop;

typedef struct yx_core_eventloop* yx_core_eventloop_ref;

/*
 init & recycle
 */
yx_bool yx_core_eventloop_init(yx_allocator allocator, yx_core_eventloop_ref eventloop);
void yx_core_eventloop_recycle(yx_core_eventloop_ref eventloop);


/*
 set global eventloop
 */
yx_core_eventloop_ref yx_core_eventloop_global(void);
void yx_core_eventloop_setGlobal(yx_core_eventloop_ref eventloop);



/*
 regist & unregist
 */

yx_bool yx_core_eventloop_regist(yx_core_eventloop_ref eventloop, yx_core_event_ref event);
yx_bool yx_core_eventloop_unregist(yx_core_eventloop_ref eventloop, yx_core_event_ref event);


/*
 process
 
 timeout 小于0，则没有超时时间。
 */
yx_bool yx_core_eventloop_process(yx_core_eventloop_ref eventloop, yx_real timeout);


/*
 run
 */
void yx_core_eventloop_run(yx_core_eventloop_ref eventloop);
void yx_core_eventloop_globalrun();



#endif /* defined(_YX_CORE_EVENTLOOP_H_) */







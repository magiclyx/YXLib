//
//  yx_core_communicator_eventdirver.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/21.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_COMMUNICATOR_EVENTDIRVER_H_
#define _YX_CORE_COMMUNICATOR_EVENTDIRVER_H_

#include "../yx_core_global.h"


/*
 这个问题件是comunicator使用eventdriver时候，使用的
 */

typedef enum yx_core_comeventdirver_opt{
    yx_core_comevent_read,
    yx_core_comevent_write,
    yx_core_comevent_connection,
}yx_core_comeventdirver_opt;




#define yx_core_comeventdriver_register(comm, os_eventdriver, opt, usrdata) \
    ( (comm)->operation.regist_event(comm, os_eventdriver, opt, (yx_value)usrdata) )


#define yx_core_comeventdriver_unregister(comm, os_eventdriver, opt) \
    ( (comm)->operation.unregist_event(comm, os_eventdriver, opt) )




#endif /* defined(_YX_CORE_COMMUNICATOR_EVENTDIRVER_H_) */




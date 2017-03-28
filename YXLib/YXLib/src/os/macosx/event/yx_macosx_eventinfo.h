//
//  yx_macosx_eventinfo.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/25.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_EVENTINFO_H_
#define _YX_MACOSX_EVENTINFO_H_

typedef enum yx_os_event_type{
    yx_os_event_type_timer = EVFILT_TIMER,
    yx_os_event_type_user = EVFILT_USER,
    yx_os_event_type_ioread = EVFILT_READ,
    yx_os_event_type_iowrite = EVFILT_WRITE,
    yx_os_event_type_process = EVFILT_PROC,
}yx_os_event_type;



typedef struct yx_os_event_info{
    yx_value ident;
    yx_os_event_type type;
    yx_uint32 subtype;
    yx_value data;
    yx_value user_data;
}yx_os_event_info;

typedef struct yx_os_event_info* yx_os_event_info_ref;



/*
 pipe
 */

#define yx_os_eventinfo_pipe_sizecanread(event_info)   ((yx_size)((event_info)->data))
#define yx_os_eventinfo_pipe_sizecanwrite(event_info)  ((yx_size)((event_info)->data))


#endif



















































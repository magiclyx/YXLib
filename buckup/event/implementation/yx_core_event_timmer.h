//
//  yx_core_event_timmer.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/21.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_EVENT_TIMMER_H_
#define _YX_CORE_EVENT_TIMMER_H_

#include "../yx_core_event.h"

typedef enum yx_core_event_timer_level{
    yx_core_event_timer_level_background,
    yx_core_event_timer_level_critical,
    yx_core_event_timer_level_absolute,
}yx_core_event_timer_level;


/*event handl*/
struct yx_core_event_timer;
typedef struct yx_core_event_timer* yx_core_event_timer_ref;
typedef void (*yx_core_event_timer_handle)(yx_core_event_timer_ref event, yx_long ident, yx_value data);


/*计时器*/
typedef struct yx_core_event_timer{
    YX_COREEVENT_STRUCT_DECLARE;
    
    yx_os_time interval;
    yx_long ident;
    yx_core_event_timer_level level;
    yx_core_event_timer_handle timer_handle;
    
    yx_value repeat:1;  /*是否重复*/
}yx_core_event_timer;



yx_bool yx_core_event_timer_init(yx_core_event_timer_ref timer_event,
                                 yx_allocator allocator,
                                 yx_os_time interval,
                                 yx_long ident,
                                 yx_core_event_timer_level level,
                                 yx_bool repead,
                                 yx_value data,
                                 yx_core_event_timer_handle handle);


yx_bool yx_core_event_timer_start(yx_core_event_timer_ref timer_event);
yx_bool yx_xore_event_timer_stop(yx_core_event_timer_ref timer_event);

#endif /* defined(_YX_CORE_EVENT_TIMMER_H_) */






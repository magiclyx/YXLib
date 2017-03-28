//
//  yx_core_event_timmer.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/21.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_event_timmer.h"
#include "yx_core_event_type.h"

#include "yx_core_eventloop.h"



void __core_event_timer_handle(yx_core_event_ref event, yx_os_event_info_ref info);



yx_bool yx_core_event_timer_init(yx_core_event_timer_ref timer_event,
                                 yx_allocator allocator,
                                 yx_os_time interval,
                                 yx_long ident,
                                 yx_core_event_timer_level level,
                                 yx_bool repead,
                                 yx_value data,
                                 yx_core_event_timer_handle handle)
{
    
    if (yx_likely(
                  yx_true == yx_core_event_init((yx_core_event_ref)timer_event, allocator, yx_core_event_type_timmer, data, __core_event_timer_handle)
                  ))
    {
        timer_event->interval = interval;
        timer_event->ident = ident;
        timer_event->level = level;
        timer_event->repeat = (yx_value)repead;
        timer_event->timer_handle = handle;
        
    }
    
    
    return yx_false;
}

yx_bool yx_core_event_timer_start(yx_core_event_timer_ref timer_event)
{
    yx_core_eventloop_ref loop = yx_core_eventloop_global();
    if (yx_likely( NULL != loop ))
    {
        return yx_os_eventdriver_add_timer(&(loop->eventdriver),
                                            timer_event->ident,
                                            timer_event->interval,
                                            timer_event->level,
                                            timer_event->repeat,
                                            (yx_value)timer_event);
    }
    
    
    return yx_false;
}

yx_bool yx_xore_event_timer_stop(yx_core_event_timer_ref timer_event)
{
    yx_core_eventloop_ref loop = yx_core_eventloop_global();
    if (yx_likely( NULL != loop ))
    {
        return yx_os_eventdriver_del_timer(&(loop->eventdriver), timer_event->ident);
    }
    
    return yx_false;
}



void __core_event_timer_handle(yx_core_event_ref event, yx_os_event_info_ref info)
{
    yx_unused(info);
    
    yx_core_event_timer_ref timer_event = (yx_core_event_timer_ref)event;
    timer_event->timer_handle(timer_event, timer_event->ident, event->data);
}



















































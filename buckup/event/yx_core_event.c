//
//  yx_core_event.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/13.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_event.h"

#include "yx_core_eventloop.h"
#include "../debug/yx_core_assert.h"


yx_bool yx_core_event_init(yx_core_event_ref event,
                           yx_allocator allocator,
                           yx_core_event_type type,
                           yx_value data,
                           yx_core_event_handle event_handle)
{
    YX_ASSERT(NULL != event);
    YX_ASSERT(NULL != allocator);
    
    
    event->allocator = allocator;
    event->type = type;
    event->data = data;
    event->event_handle = event_handle;
    
    
    return yx_true;
}






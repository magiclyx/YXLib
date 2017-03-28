//
//  yx_core_event.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/13.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_eventloop.h"
#include "../debug/yx_core_assert.h"

static yx_core_eventloop_ref _global_event_loop = NULL;


static void __event_calback(yx_os_event_info_ref event_info);
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

yx_bool yx_core_eventloop_init(yx_allocator allocator, yx_core_eventloop_ref eventloop)
{
    YX_ASSERT(NULL != eventloop);
    
    if(yx_unlikely( yx_os_eventdriver_init(allocator, &(eventloop->eventdriver), __event_calback, 1024) ))
        return yx_false;
    
    yx_core_listnode_sentinelInit(&(eventloop->list_head));
    
    eventloop->allocator = allocator;
   
    return yx_true;
}

void yx_core_eventloop_recycle(yx_core_eventloop_ref eventloop)
{
    YX_ASSERT(NULL != eventloop);
    
    if (_global_event_loop == eventloop)
        _global_event_loop = NULL;
    
    yx_os_eventdriver_recycle(&(eventloop->eventdriver));
    
}

yx_core_eventloop_ref yx_core_eventloop_global(void)
{
    return _global_event_loop;
}

void yx_core_eventloop_setGlobal(yx_core_eventloop_ref eventloop)
{
    _global_event_loop = eventloop;
}


yx_bool yx_core_eventloop_regist(yx_core_eventloop_ref eventloop, yx_core_event_ref event)
{
    YX_ASSERT(NULL != event);
    YX_ASSERT(NULL != eventloop);
    
    yx_core_listnode_insertTail(&(eventloop->list_head), &(event->node));
    
    
    return yx_false;
}
yx_bool yx_core_eventloop_unregist(yx_core_eventloop_ref eventloop, yx_core_event_ref event)
{
    
    YX_ASSERT(NULL != event);
    YX_ASSERT(NULL != eventloop);
    
    
    yx_core_listnode_del(&(event->node));

    
    return yx_false;
}



yx_bool yx_core_eventloop_process(yx_core_eventloop_ref eventloop, yx_real timeout)
{
    yx_os_critime_nsec time;
    yx_os_critime_nsec* time_ptr = NULL;
    
    if (timeout > 0)
    {
        time.sec = (yx_long)timeout;
        time.nsec = (yx_long)((timeout - (yx_real)(yx_long)timeout) * YX_NSEC_PER_SEC);
        
        time_ptr = &time;
    }
    
    
    return yx_os_eventdriver_process(&(eventloop->eventdriver), time_ptr);
}

void yx_core_eventloop_run(yx_core_eventloop_ref eventloop)
{
    for (;;)
    {
        yx_core_eventloop_process(eventloop, -1);
        
        //:~ TODO
    }
}

void yx_core_eventloop_globalrun()
{
    
    yx_core_eventloop_ref eventloop = yx_core_eventloop_global();
    assert(NULL != eventloop);
    
    for (;;)
    {
        yx_core_eventloop_process(eventloop, -1);
        
        //:~ TODO
    }
}







////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
static void __event_calback(yx_os_event_info_ref event_info)
{
    yx_core_event_ref event = (yx_core_event_ref)(event_info->user_data);
    YX_ASSERT(NULL != event);
    
    /* TODO 把result设置到event里 */
    event->event_handle(event, event_info);
}





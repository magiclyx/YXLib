//
//  yx_macosx_event.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/6.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_event.h"
#include "../yx_macosx_global.h"
#include "../system/yx_macosx_system.h"

yx_bool yx_os_eventdriver_init(yx_allocator allocator, yx_os_eventdriver_ref event, void (*event_calback)(yx_os_event_info_ref event_info), yx_int max_event_num)
{
    assert(NULL != event);
    assert(NULL != event_calback);
    
    int kq = -1;
    struct kevent* event_list = NULL;
    
    if (max_event_num <= 0)
        max_event_num = yx_os_system_maxfileopened();
    
    
    event_list = yx_allocator_alloc(allocator, sizeof(struct kevent) * (yx_size)max_event_num);
    if (yx_unlikely(NULL == event_list))
        goto errout;
    
    kq = kqueue();
    if (yx_unlikely(-1 == kq))
        goto errout;
    
    
    event->allocator = allocator;
    event->max_event_num = max_event_num;
    event->event_num = 0;
    event->kq = kq;
    event->event_list = event_list;
    event->event_calback = event_calback;
    
    
    return yx_true;
    
    
errout:
    if (NULL != event_list)
        yx_allocator_free(allocator, event_list);
    
    if (-1 != kq)
        close(kq);
    
    
    return yx_false;
}

void yx_os_eventdriver_recycle(yx_os_eventdriver_ref event)
{
    assert(NULL != event);
    
    close(event->kq);
    
    yx_allocator_free(event->allocator, event->event_list);
}


yx_bool yx_os_eventdriver_add_timer(yx_os_eventdriver_ref event, yx_long ident, yx_os_time time, yx_int prec_level, yx_bool repeat, yx_value userdata)
{
    uint32_t flag = 0;
    uint32_t fflags = 0;
    yx_ulong ltime;
    struct kevent change;
    
    
    /*
     set flag value
     */
    flag = EV_ADD|EV_ENABLE;
    if (! repeat)
        flag |= EV_ONESHOT;

    
    /*
     set fflag value
     */
    
    do{
        
        //seconds level
        yx_real ftime = time;
        ltime = (yx_ulong)time;
        yx_real delta = ftime - ((yx_real)ltime);
        if (delta < 0.1f  &&  delta > -0.1f)
        {
            fflags |= NOTE_SECONDS;
            break;
        }
        
        
        //microseconds level
        ftime = time * 1000000.0f;
        ltime = (yx_ulong)(ftime);
        delta = ftime - ((yx_real)ltime);
        
        if (delta < 0.1f  &&  delta > -0.1f)
        {
            fflags |= NOTE_USECONDS;
            break;
        }
        
        
        
        //nanoseconds level
        ltime = (yx_ulong)(ftime * 1000.0f);
        fflags |= NOTE_NSECONDS;
        
        
    }while (0);
    
    
    
    switch (prec_level) {
        case 1:
            fflags |= NOTE_BACKGROUND;
            break;
        case 2:
            fflags |= NOTE_CRITICAL;
            break;
        case 3:
            fflags |= NOTE_ABSOLUTE;
            break;
        default:
            fflags |= NOTE_BACKGROUND;
            break;
    }
    
    
    EV_SET(&change, ident, EVFILT_TIMER, flag, fflags, ltime, (void*)userdata);
    
    int ret = kevent(event->kq, &change, 1, NULL, 0, NULL);
    if (yx_unlikely(-1 == ret)){
        return yx_false;
    }
    
    
//    else
//    {
        event->event_num++;
        //:~TODO add log to event num over flow
    //    if (event->event_num > event->max_event_num)
    //    {
    //        /*log*/
    //    }
//    }
    
    
    
    return yx_true;

}

yx_bool yx_os_eventdriver_del_timer(yx_os_eventdriver_ref event, yx_long ident)
{
    assert(NULL != event);
    
    struct kevent change;
    
    EV_SET(&change, ident, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
    
    int ret = kevent(event->kq, &change, 1, NULL, 0, NULL);
    if (yx_likely(-1 != ret))
    {
        event->event_num--;
    }
    else
    {
        return yx_false;
    }

    return yx_true;
}


yx_bool yx_os_eventdriver_add_custom(yx_os_eventdriver_ref event, yx_long ident)
{
    assert(NULL != event);
    
    struct kevent change;
    uint16_t flags = EV_ADD|EV_CLEAR;
    
    
    flags |= EV_ONESHOT; //do not repead
    
    
    EV_SET(&change, ident, EVFILT_USER, flags, 0, 0, NULL);
    
    int ret = kevent(event->kq, &change, 1, NULL, 0, NULL);
    if (yx_likely(-1 != ret))
    {
        event->event_num++;
        //:~TODO add log to event num over flow
        //    if (event->event_num > event->max_event_num)
        //    {
        //        /*log*/
        //    }
        
    }
    else
    {
        return yx_false;
    }
    

    
    return yx_true;

}

yx_bool yx_os_eventdriver_del_custom(yx_os_eventdriver_ref event, yx_long ident)
{
    assert(NULL != event);
    
    struct kevent change;
    
    EV_SET(&change, ident, EVFILT_USER, EV_DELETE, 0, 0, NULL);
    
    int ret = kevent(event->kq, &change, 1, NULL, 0, NULL);
    if (yx_likely(-1 != ret))
    {
        event->event_num--;
    }
    else
    {
        return yx_false;
    }

    
    return yx_true;
}

yx_bool yx_os_eventdriver_send_custom(yx_os_eventdriver_ref event, yx_long ident, yx_value userdata)
{
    assert(NULL != event);
    
    struct kevent change;

    EV_SET(&change, ident, EVFILT_USER, 0, NOTE_TRIGGER|NOTE_FFCOPY, 0, (void*)userdata);
    
    int ret = kevent(event->kq, &change, 1, NULL, 0, NULL);
    if (yx_unlikely(-1 == ret))
        return yx_false;
    
    return yx_true;
}




yx_bool yx_os_eventdriver_add_file_read(yx_os_eventdriver_ref event, yx_os_file file, yx_value usrdata)
{
    assert(NULL != event);
    
    struct kevent change;
    uint16_t flags = EV_ADD|EV_CLEAR|EV_ENABLE;
    
    
    EV_SET(&change, file, EVFILT_READ, flags, 0, 0, (void*)usrdata);
    
    int ret = kevent(event->kq, &change, 1, NULL, 0, NULL);
    if (yx_unlikely(-1 == ret))
        return yx_false;
    
    return yx_true;
}

yx_bool yx_os_eventdriver_add_file_write(yx_os_eventdriver_ref event, yx_os_file file, yx_value usrdata)
{
    assert(NULL != event);
    
    struct kevent change;
    uint16_t flags = EV_ADD|EV_CLEAR|EV_ENABLE;
    
    
    EV_SET(&change, file, EVFILT_WRITE, flags, 0, 0, (void*)usrdata);
    
    int ret = kevent(event->kq, &change, 1, NULL, 0, NULL);
    if (yx_likely(-1 != ret))
    {
        event->event_num++;
        //:~TODO add log to event num over flow
        //    if (event->event_num > event->max_event_num)
        //    {
        //        /*log*/
        //    }
    }
    else
    {
        return yx_false;
    }
    
    return yx_true;

}

yx_bool yx_os_eventdriver_del_file_read(yx_os_eventdriver_ref event, yx_os_file file)
{
    assert(NULL != event);
    
    struct kevent change;
    
    EV_SET(&change, file, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    
    int ret = kevent(event->kq, &change, 1, NULL, 0, NULL);
    if (yx_likely(-1 != ret))
    {
        event->event_num--;
    }
    else
    {
        return yx_false;
    }
    
    return yx_true;

}

yx_bool yx_os_eventdriver_del_file_write(yx_os_eventdriver_ref event, yx_os_file file)
{
    assert(NULL != event);
    
    struct kevent change;
    
    EV_SET(&change, file, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    
    int ret = kevent(event->kq, &change, 1, NULL, 0, NULL);
    if (yx_likely(-1 != ret))
    {
        event->event_num--;
    }
    else
    {
        return yx_false;
    }
    
    return yx_true;

}


yx_bool yx_os_eventdriver_add_process(yx_os_eventdriver_ref event, yx_os_prochandle process_handle, yx_value userdata)
{
    assert(NULL != event);
    
    struct kevent change;
//    uint16_t flags = EV_ADD|EV_CLEAR|EV_ENABLE|EV_ONESHOT;
    uint16_t flags = EV_ADD|EV_ONESHOT;
    
    
    EV_SET(&change, process_handle, EVFILT_PROC, flags, NOTE_EXIT, 0, (void*)userdata);
    
    int ret = kevent(event->kq, &change, 1, NULL, 0, NULL);
    if (yx_likely(-1 != ret))
    {
        event->event_num++;
        //:~TODO add log to event num over flow
        //    if (event->event_num > event->max_event_num)
        //    {
        //        /*log*/
        //    }
    }
    else
    {
        return yx_false;
    }
    

    
    return yx_true;

}

yx_bool yx_os_eventdriver_del_process(yx_os_eventdriver_ref event, yx_os_prochandle process_handle)
{
    assert(NULL != event);
    
    struct kevent change;
    
    EV_SET(&change, process_handle, EVFILT_PROC, EV_DELETE, 0, 0, NULL);
    
    int ret = kevent(event->kq, &change, 1, NULL, 0, NULL);
    if (yx_likely(-1 != ret))
    {
        event->event_num--;
    }
    else
    {
        return yx_false;
    }
    
    return yx_true;

}




yx_bool yx_os_eventdriver_process(yx_os_eventdriver_ref event, yx_os_critime_nsec* cri_nsec)
{
    
    
    struct timespec time;
    struct timespec* time_pt = NULL;
    
    if (NULL != cri_nsec){
        time.tv_sec = cri_nsec->sec;
        time.tv_nsec = cri_nsec->nsec;
        
        time_pt = &time;
    }
    
    
    int process_num = kevent(event->kq, NULL, 0, event->event_list, event->event_num, time_pt);
    if (yx_unlikely(process_num == -1))
        return yx_false;
    
    
    yx_os_event_info info;
    for (int i=0; i < process_num; i++)
    {
        if (EV_ERROR == event->event_list[i].flags)
        {
            //errno = events[i].data
            continue;
        }
        
        info.ident = event->event_list[i].ident;
        info.type = event->event_list[i].filter;
        info.subtype = event->event_list[i].fflags;
        info.user_data = (yx_value)event->event_list[i].udata;
        info.data = (yx_value)event->event_list[i].data;
        
        event->event_calback(&info);
    }
    
    return yx_true;
}




//
//  yx_macosx_event.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/6.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_EVENT_H_
#define _YX_MACOSX_EVENT_H_

#include "../basictypes/yx_macosx_types.h"
#include "../time/yx_macosx_time.h"
#include "../allocator/yx_macosx_allocator.h"
#include "../file/yx_macosx_file_def.h"
#include "../time/yx_macosx_critime.h"
#include "../pipe/yx_macosx_pipe.h"
#include "../process/yx_macosx_process.h"

#include "yx_macosx_eventinfo.h"


/*
 警告， kqueue是不会被子进程继承的
 */




typedef struct yx_os_eventdirver{
    int kq;
    yx_allocator allocator;
    struct kevent* event_list;
    yx_int max_event_num;
    yx_int event_num;
    
    void (*event_calback)(yx_os_event_info_ref event_info);
}yx_os_eventdirver;

typedef struct yx_os_eventdirver* yx_os_eventdriver_ref;





yx_bool yx_os_eventdriver_init(yx_allocator allocator, yx_os_eventdriver_ref event, void (*event_calback)(yx_os_event_info_ref event_info), yx_int max_event_num);
void yx_os_eventdriver_recycle(yx_os_eventdriver_ref event);


////////////////////////////////////////////////////////////////////////////////////////////////////
/* process */
////////////////////////////////////////////////////////////////////////////////////////////////////

yx_bool yx_os_eventdriver_process(yx_os_eventdriver_ref event, yx_os_critime_nsec* cri_nsec);



#pragma mark file event
/**
 *  for sockets:
 *      data contains the size of the listen backlog
 *
 *  for vnodes
 *      data contains the offset from current position to end of file, and may be negative
 *
 *  for Fifos, Pipes
 *      data contains the number of bytes available.
 *
 */
yx_bool yx_os_eventdriver_add_file_read(yx_os_eventdriver_ref event, yx_os_file file, yx_value usrdata);
yx_bool yx_os_eventdriver_add_file_write(yx_os_eventdriver_ref event, yx_os_file file, yx_value usrdata);
yx_bool yx_os_eventdriver_del_file_read(yx_os_eventdriver_ref event, yx_os_file file);
yx_bool yx_os_eventdriver_del_file_write(yx_os_eventdriver_ref event, yx_os_file file);


#pragma mark timer event
/**
 *  register a timer
 *
 *  @param event        yx_os_event reference
 *  @param time         time in seconds(yx_real value)
 *  @param prec_level   precision level
                             1->system has extra leeway to coalesce this timer
                             2->system makes a best effort to fire this timer as scheduled
                             3->data is an absolute timeout
 *  @param repeat       whether the timer will repeat
 *  @param userdata     userdata
 *
 *
 *  @return yx_true if sucess
 */
yx_bool yx_os_eventdriver_add_timer(yx_os_eventdriver_ref event, yx_long ident, yx_os_time time, yx_int prec_level, yx_bool repeat, yx_value userdata);
yx_bool yx_os_eventdriver_del_timer(yx_os_eventdriver_ref event, yx_long ident);



#pragma mark user event
/*
 * custom event
 */
yx_bool yx_os_eventdriver_add_custom(yx_os_eventdriver_ref event, yx_long ident);
yx_bool yx_os_eventdriver_del_custom(yx_os_eventdriver_ref event, yx_long ident);
yx_bool yx_os_eventdriver_send_custom(yx_os_eventdriver_ref event, yx_long ident, yx_value userdata);



#pragma mark process event
/*
 process event
 */

yx_bool yx_os_eventdriver_add_process(yx_os_eventdriver_ref event, yx_os_prochandle process_handle, yx_value userdata);
yx_bool yx_os_eventdriver_del_process(yx_os_eventdriver_ref event, yx_os_prochandle process_handle);

////////////////////////////////////////////////////////////////////////////////////////////////////
/* pipe */
////////////////////////////////////////////////////////////////////////////////////////////////////

/*add*/
#define yx_os_eventdriver_add_pipe_read(event, pipe, userdata) \
    ( yx_os_pipe_canread(pipe)? yx_os_eventdriver_add_file_read((event), yx_os_pipe_readHandle(pipe), userdata) : yx_false )

#define yx_os_eventdriver_add_pipe_write(event, pipe, userdata) \
    ( yx_os_pipe_canwrite(pipe)? yx_os_eventdriver_add_file_read((event), yx_os_pipe_writeHandle(pipe), userdata) : yx_false )

/*del*/
#define yx_os_eventdriver_del_piple_read(event, pipe) \
    ( yx_os_pipe_canread(pipe)? yx_os_eventdriver_del_file_read((event), yx_os_pipe_readHandle(pipe)) : yx_false )

#define yx_os_eventdriver_del_piple_write(event, pipe) \
    ( yx_os_pipe_canwrite(pipe)? yx_os_eventdriver_del_file_write((event), yx_os_pipe_writeHandle(pipe)) : yx_false )


/*enable*/
#define yx_os_eventdriver_enable_pipe_read(event, pipe, userdata) \
    yx_os_eventdriver_add_pipe_read(event, pipe, userdata, yx_true)

#define yx_os_eventdriver_enable_pipe_write(event, pipe, userdata) \
    yx_os_eventdriver_add_pipe_write(event, pipe, userdata, yx_true)

/*disable*/
#define yx_os_eventdriver_disable_pipe_read(event, pipe, userdata) \
    yx_os_eventdriver_add_pipe_read(event, pipe, userdata, yx_false)

#define yx_os_eventdriver_disable_pipe_write(event, pipe, userdata) \
    yx_os_eventdriver_add_pipe_write(event, pipe, userdata, yx_false)



////////////////////////////////////////////////////////////////////////////////////////////////////
/* process */
////////////////////////////////////////////////////////////////////////////////////////////////////

yx_bool yx_os_eventdriver_process(yx_os_eventdriver_ref event, yx_os_critime_nsec* cri_nsec);


#endif /* defined(_YX_MACOSX_EVENT_H_) */






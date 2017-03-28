//
//  yx_macosx_process_status.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/13.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_PROCESS_STATUS_H_
#define _YX_MACOSX_PROCESS_STATUS_H_

#include "yx_macosx_process.h"

#define yx_os_procstatus_unknown -3
#define yx_os_procstatus_invald  -2   /*invalid process (not init)*/
#define yx_os_procestatus_error  -1   /*error to get the status*/
#define yx_os_procstatus_running  0   /*not exit*/


typedef int yx_os_procexit_status;  //process state


yx_os_procexit_status yx_os_process_exitstatus(yx_os_process_ref process);

////////////////////////////////////////////////////////////////////////////////////////////////////
/*is status*/
////////////////////////////////////////////////////////////////////////////////////////////////////

/*if exit*/
#define yx_os_process_isExit(exit_status) (exit_status > 0)
/*True if the process terminated normally by a call to _exit(2) or exit(3)*/
#define yx_os_process_isNormalExit(exit_status) (yx_os_process_isExit(exit_status) &&  WIFEXITED(exit_status))
/*True if the process terminated due to receipt of a signal.*/
#define yx_os_process_isSignExit(exit_status) (yx_os_process_isExit(exit_status) &&  WIFSIGNALED(exit_status))


yx_bool yx_os_process_isPendding(yx_os_process_ref process);

////////////////////////////////////////////////////////////////////////////////////////////////////
/*exit info*/
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 evaluates to the low-order 8 bits of the argument passed to _exit(2) or exit(3) by the child.
 
 if not normal exit, return -1
 */
#define yx_os_process_exitcode(exit_status) (yx_os_process_isNormalExit(exit_status)? WEXITSTATUS(exit_status) : -1)

/*
 evaluates as true if the termina- tion of the process was accompanied by the creation of a core
 file containing an image of the process when the signal was
 received.
 
 if not exit by signal, return -1
 */
#define yx_os_process_exitsign(exit_status) (yx_os_process_isSignExit(exit_status)? WTERMSIG(exit_status) : -1)

#endif /* defined(_YX_MACOSX_PROCESS_STATUS_H_) */



















































//
//  yx_macosx_process.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/7.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_PROCESS_H_
#define _YX_MACOSX_PROCESS_H_

#include "yx_macosx_process_info.h"
#include "../yx_macosx_global.h"
#include "../basictypes/yx_macosx_types.h"


#define yx_os_process_maxname_len   MAXCOMLEN  //进程名字的最大长度




typedef pid_t yx_os_prochandle;

typedef struct yx_os_process {
//    yx_os_prochandle process_id;
    yx_int32 process_id;
    yx_int32 need_close_handle:1;
}yx_os_process;

typedef struct yx_os_process* yx_os_process_ref;


#pragma mark general&handle
////////////////////////////////////////////////////////////////////////////////////////////////////
/* general and handle*/
////////////////////////////////////////////////////////////////////////////////////////////////////
#define yx_os_process_initnull(process)     ((process)->process_id = -1)

/*get current process's handle*/
#define yx_os_process_current_handle() \
getpid()



/*init current process struct*/
yx_forceinline
yx_bool yx_os_process_current_init(yx_os_process_ref process)
{
    assert(NULL != process);
    process->process_id = yx_os_process_current_handle();
    process->need_close_handle = 0;
    
    return yx_true;
}


yx_forceinline
yx_bool yx_os_process_current_initCurrent(yx_os_process_ref process)
{
    assert(NULL != process);
    
    process->process_id = yx_os_process_current_handle();
    process->need_close_handle = 0;
    
    return yx_true;
}





/*get handle form process*/
#define yx_os_process_handle(process) \
    ((process)->process_id)




#pragma mark process title
////////////////////////////////////////////////////////////////////////////////////////////////////
/* process title */
////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 *  set the process title
 *
 *  1. must call the yx_os_environment_save_launch_parameter to set the argc and argv.
 *  2. use std-c alloator a space in this function and will not release it until the end of the process
 */
yx_bool yx_os_process_set_title(const yx_char* title);
const yx_char* yx_os_process_title(void);


#pragma mark child process
////////////////////////////////////////////////////////////////////////////////////////////////////
/* child process */
////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 create the process
 
 if name is not NULL, set it as the child-process's title
 
 process  子进程中为父进程的process信息
          父进程中为子进程的process信息
   
 
 子进程关闭后，父进程需要调用 yx_os_process_closeHandleIfExit 来关闭子进程句柄
 */
yx_bool yx_os_process_fork(yx_os_process_ref process, const yx_char* name, yx_bool* isChild);
#define yx_os_process_isValid(process)  (-1 != (process)->process_id)

yx_bool yx_os_process_closeHandle(yx_os_process_ref process, yx_bool wait);
#define yx_os_process_closeHandleIfExit(process) yx_os_process_closeHandle(process, yx_false)



#pragma mark exit the process
////////////////////////////////////////////////////////////////////////////////////////////////////
/* 退出进程 */
////////////////////////////////////////////////////////////////////////////////////////////////////

#define yx_os_process_exit(code) exit(code)
#define yx_os_process_exit_normal() yx_os_process_exit(0)




#pragma mark current process info
////////////////////////////////////////////////////////////////////////////////////////////////////
/* current proces info*/
////////////////////////////////////////////////////////////////////////////////////////////////////


/*get current process's info*/
#define yx_os_process_current_info(processinfo) \
yx_os_processinfo_withpid(processinfo, {yx_os_process_current_handle();})



#endif /* defined(_YX_MACOSX_PROCESS_H_) */






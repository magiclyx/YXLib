//
//  yx_core_process.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/12.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_PROCESS_H_
#define _YX_CORE_PROCESS_H_

#include "../yx_core_global.h"

#include "../string/yx_core_cstring.h"
#include "../communicator/yx_core_communicator.h"

typedef struct yx_core_process{
    yx_os_process process;
    yx_os_procexit_status status; /*获取当前进程状态的时候，不要直接取，要调用函数yx_core_process_status*/
    yx_char name[yx_os_process_maxname_len + 1]; /*应用自进程title指针中的字符串*/
}yx_core_process;

typedef struct yx_core_process* yx_core_process_ref;


yx_bool yx_core_process_init(yx_core_process_ref process);
yx_bool yx_core_process_recycle(yx_core_process_ref process, yx_bool wait); //是否等待进程结束，如果为NO,且进程没结束，则回收失败

/*
 process,  子进程将返回父进程的process信息
           父进程将返回子进程的process信息
 
 communicator, 将一个communicator绑定到两个通信进程
 */
yx_bool yx_core_process_fork(yx_core_process_ref process, const yx_char* name, yx_bool* isChild);


/*
 获取当前进程状态
 */
yx_os_procexit_status yx_core_process_status(yx_core_process_ref process);

// 进程是否退出
#define yx_core_process_isExit(process)   yx_os_process_isExit(yx_core_process_status(process))
//退出代码, 实际为8bit数值，如果获得失败返回-1
#define yx_core_process_exitCode(process) yx_os_process_exitcode(yx_core_process_status(process))
//退出信号，如果获取失败返回-1
#define yx_core_process_exitSign(process) yx_os_process_exitsign(yx_core_process_status(process))


#endif /* defined(_YX_CORE_PROCESS_H_) */



















































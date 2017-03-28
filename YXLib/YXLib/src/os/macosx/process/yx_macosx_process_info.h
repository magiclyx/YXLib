//
//  yx_macosx_process_info.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/11.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_PROCESS_INFO__
#define _YX_MACOSX_PROCESS_INFO__

#include "../basictypes/yx_macosx_types.h"
#include "../time/yx_macosx_time.h"
#include "../allocator/yx_macosx_allocator.h"


typedef struct yx_os_process* yx_os_process_ref;





typedef struct kinfo_proc yx_os_processinfo;
typedef struct kinfo_proc* yx_os_processinfo_ref;


yx_bool yx_os_processinfo_withpid(yx_os_processinfo_ref processinfo, yx_os_process_ref process);


/*process id*/
#define yx_os_processinfo_getpid(processinfo)            (processinfo->kp_proc.p_pid)
#define yx_os_processinfo_getppid(processinfo)           (processinfo->kp_eproc.e_ppid)
#define yx_os_processinfo_get_current_pid()              getpid()

/*owner id*/
#define yx_os_processinfo_geteuid(processinfo)           (processinfo->kp_eproc.e_ucred.cr_uid)
#define yx_os_processinfo_getuid(processinfo)            (processinfo->kp_eproc.e_pcred.p_svuid)
#define yx_os_processinfo_getuid_real(processinfo)       (processinfo->kp_eproc.e_pcred.p_ruid)

#define yx_os_processinfo_getgid(processinfo)            (processinfo->kp_eproc.e_pcred.p_svgid)
#define yx_os_processinfo_getgid_real(processinfo)       (processinfo->kp_eproc.e_pcred.p_rgid)

/*process stat*/
#define yx_os_processinfo_getname(processinfo)           (processinfo->kp_proc.p_comm)
#define yx_os_processinfo_getstat(processinfo)           (processinfo->kp_proc.p_stat)
#define yx_os_processinfo_isDebug(processinfo)           (processinfo->kp_proc.p_debugger)
#define yx_os_processinfo_isSuspend(processinfo)         (processinfo->kp_proc.sigwait)
#define yx_os_processinfo_getPriority(processinfo)       (processinfo->kp_proc.p_priority)

/*cpu*/
#define yx_os_processinfo_getcpuAverage(processinfo);    (processinfo->kp_proc.p_estcpu)
#define yx_os_processinfo_getcpuTicket(processinfo)      (processinfo->kp_proc.p_cpticks)


/*launch time*/
#define yx_os_processinfo_getstarttime_usec(processinfo) ((processinfo->kp_proc.p_starttime.tv_sec * YX_USEC_PER_SEC) + processinfo->kp_proc.p_starttime.tv_usec)
#define yx_os_processinfo_getstarttime_sec(processinfo)  (processinfo->kp_proc.p_starttime.tv_sec)

yx_forceinline
yx_os_time yx_os_processinfo_getstarttime(yx_os_processinfo_ref processinfo) {
    return ((yx_real)(processinfo->kp_proc.p_starttime.tv_usec) / (yx_real)YX_USEC_PER_SEC);
}

/*time since last block*/
#define yx_os_processinfo_getTimeSinceLastBlock(processinfo)    (processinfo->proc.kp_proc.p_slptime)



////////////////////////////////////////////////////////////////////////////////////////////////////
/*get a list of process info*/
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct yx_os_processinfo_buf{
    yx_os_processinfo_ref info_buf;
    yx_size buf_size;
    yx_allocator allocator;
    yx_int count;
}yx_os_processinfo_buf;

typedef struct yx_os_processinfo_buf* yx_os_processinfo_buf_ref;


yx_bool yx_os_processinfobuf_init(yx_allocator allocator, yx_os_processinfo_buf_ref processinfo_buf);
void yx_os_processinfobuf_recycle(yx_os_processinfo_buf_ref processinfo_buf);

yx_bool yx_os_processinfobuf_current(yx_os_processinfo_buf_ref processinfo_buf);


#endif /* defined(_YX_MACOSX_PROCESS_INFO__) */







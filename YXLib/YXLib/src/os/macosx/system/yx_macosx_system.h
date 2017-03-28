//
//  yx_macosx_system.h
//  YXLib
//
//  Created by Yuxi Liu on 9/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef YXLib_yx_macosx_system_h
#define YXLib_yx_macosx_system_h

#include "../yx_macosx_global.h"

#include "../err/yx_macosx_err.h"
#include "../basictypes/yx_macosx_types.h"
#include "../time/yx_macosx_time.h"



#pragma mark maxpathlen
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
#define YX_OS_MAXPATH_LEN (1024 - 1) //a macro for max path

/*guess the mat path support in systems.(often not correct)*/
yx_long yx_os_system_maxpathlen(void); //get the real max path length



#pragma mark general
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/*The machine class, return NULL if failure*/
const yx_char* yx_os_system_machineInfo(void);

/*The machine model, return NULL if failure*/
const yx_char* yx_os_system_modelInfo(void);


/*The number of physical processors available in the current power management mode. return -1 if failure*/
int yx_os_system_physicalcpuNum(void);
/*The maximum number of physical processors that could be available this boot. return -1 if failure*/
int yx_os_system_physicalcpuMaxNum(void);
/*The number of logical processors available in the current power management mode. return -1 if failure*/
int yx_os_system_logicalcpuNum(void);
/*The maximum number of logical processors that could be available this boot. return -1 if failure*/
int yx_os_system_logicalcpuMaxNum(void);
/*CPU Frequency*/
int yx_os_system_cpufrequency(void);
/*cpu brand*/
const yx_char* yx_os_system_cpubrand(void);
/*cache line size*/
int yx_os_system_cpucachelinesize(void);
/*l1 I cache line size*/
int yx_os_system_cpucachelinesize_L1_I(void);
/*l1 D cache line size*/
int yx_os_system_cpucachelinesize_L1_D(void);
/*l2 cache line size*/
int yx_os_system_cpucachelinesize_L2(void);
/*l3 cache line size*/
int yx_os_system_cpucachelinesize_L3(void);


/*The byteorder (4,321, or 1,234).*/
int yx_os_system_byteorder(void);


/*physical memory represented by a 32-bit integer (for backward compatibility). return -1 if failure*/
yx_int32 yx_os_system_memsize32(void);
/*physical memory represented by a 64-bit integer. return (yx_size)-1 if failure*/
yx_ulonglong yx_os_system_memsize(void);
/*non-kernel memory.*/
yx_ulonglong yx_os_system_usermem(void);
/*memused*/
yx_ulonglong yx_os_system_memused(void);
/*mem freed*/
yx_ulonglong yx_os_system_memfree(void);

/*virtual memory*/
yx_ulonglong yx_os_system_vmemsize(void);
/*virtual memory used*/
yx_ulonglong yx_os_system_vmemused(void);
/*virtual memory used by current process*/
yx_ulonglong yx_os_system_vmemused_curproc(void);


/*The software page size.*/
yx_int yx_os_system_pagesize(void);

/*get free disk space on disk*/
yx_ulonglong yx_os_system_freediskspace(const yx_char* path);
/*get total disk space on disk*/
yx_ulonglong yx_os_system_totaldisspace(const yx_char* path);




#pragma mark kernel
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*The maximum bytes of argument to execve(2).*/
yx_size yx_os_system_maxarg(void);


/*A struct timeval structure is returned.  This structure contains the time that the system was booted.*/
yx_os_time yx_os_system_boottime(void);   //real
yx_long yx_os_system_boottime_sec(void);  //second
yx_long yx_os_system_boottime_usec(void); //microseconds



/*system clock info, linux only !!*/
const struct clockinfo* yx_os_system_clockinfo(void);

/*Get or set the host id.*/
yx_int yx_os_system_hostid(void);
/*Get or set the hostname.*/
const yx_char* yx_os_system_hostname(void);

/*
 The maximum number of files that may be open for a single process.  This limit only applies to
 processes with an effective uid of nonzero at the time of the open request.  Files that have
 already been opened are not affected if the limit or the effective uid is changed.
 */
yx_int yx_os_system_maxfileopened_perproc(void);
/*The maximum number of files that may be open in the system.*/
yx_int yx_os_system_maxfileopened(void);


/*
 The maximum number of concurrent processes the system will allow for a single effective uid.
 This limit only applies to processes with an effective uid of nonzero at the time of a fork
 request.  Processes that have already been started are not affected if the limit is changed.
 */
yx_int yx_os_system_maxprocess_peruid(void);
/*The maximum number of concurrent processes the system will allow.*/
yx_int yx_os_system_maxprocess(void);
/*max vnodes*/
yx_int yx_os_system_maxfileNum(void);



/*compile time info*/
const yx_char* yx_os_system_kernerVersion(void);
/*OS release date */
yx_int yx_os_system_releasedate(void);
/*system release*/
const yx_char* yx_os_release(void);
/*The system revision.*/
yx_int yx_os_system_revision(void);
/*system type*/
const yx_char* yx_os_system_type(void);


#pragma mark user
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/*To retrieve the standard search path for the system utilities*/
const yx_char* yx_os_system_searchpath(void);

/*Load average structure. linux only*/
const struct loadavg* yx_os_system_loadavg(void);

#endif



















































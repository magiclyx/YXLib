//
//  yx_macosx_time.h
//  YXLib
//
//  Created by Yuxi Liu on 9/30/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_TIME_H_
#define _YX_MACOSX_TIME_H_


#include "../yx_macosx_global.h"
#include "../basictypes/yx_macosx_types.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/*define*/
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 基本
 */

typedef yx_uint64 yx_os_tick;  /*ticket*/
typedef yx_real   yx_os_time;  /*时间*/
typedef struct tm yx_os_date;  /*日期*/

typedef struct timeval yx_os_timeval; /*posix 指定的时间结构, 第二个属性是毫秒*/
typedef struct timespec yx_os_timespec; /*posix 指定的时间结构, 第二个属性是纳秒*/

/*
 时间细分
 */
typedef yx_long yx_os_second;   /*秒*/
typedef yx_long yx_os_msecond;   /*秒*/
typedef yx_long yx_os_usecond;  /*微秒*/
typedef yx_long yx_os_nsecond;  /*纳秒*/



////////////////////////////////////////////////////////////////////////////////////////////////////
/*convert*/
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *  1 SEC  = 1000 MSEC    1 second      = 1000 milliseconds
 *  1 MSEC = 1000 USEC    1 millsecond  = 1000 microseconds
 *  1 USEC = 1000 NSEC    1 microsecond = 1000 nanoseconds
 */

#define YX_MSEC_PER_SEC    1000ll        /* milliseconds per second */
#define YX_USEC_PER_SEC    1000000ll     /* microseconds per second */
#define YX_NSEC_PER_SEC    1000000000ll  /* nanoseconds per second */

#define YX_NANO_PER_USEC   1000ll       /* nanoseconds per microsecond */
#define YX_NANO_PER_MSEC   1000000ll    /* nanoseconds per millisecond */


/*
 convert
 */
#define yx_os_sec2msec(t)  (t * YX_MSEC_PER_SEC)  /*秒->毫秒*/
#define yx_os_sec2usec(t)  (t * YX_USEC_PER_SEC)  /*秒->微秒*/
#define yx_os_sec2nsec(t)  (t * YX_NSEC_PER_SEC)  /*秒->纳秒*/

#define yx_os_nsec2sec(t)   (t / YX_NSEC_PER_SEC)  /*纳秒->秒*/
#define yx_os_nsec2usec(t)  (t / YX_NANO_PER_USEC) /*纳秒->微秒*/
#define yx_os_nsec2msec(t)  (t / YX_NANO_PER_MSEC) /*纳秒->毫秒*/


#define yx_os_msec2sec(t)   (t / YX_MSEC_PER_SEC) /*毫秒->秒*/
#define yx_os_msec2usec(t)  (t * 1000ll)         /*毫秒->微秒*/
#define yx_os_msec2nsec(t)  (t * 1000000ll)      /*毫秒->纳秒*/


#define yx_os_nsec2sec(t)   (t / YX_NSEC_PER_SEC)  /*纳秒->秒*/
#define yx_os_nsec2msec(t)  (t / YX_NANO_PER_MSEC) /*纳秒->毫秒*/
#define yx_os_nsec2usec(t)  (t / YX_NANO_PER_USEC) /*纳秒->微秒*/





extern yx_os_time _yx_os_ticket_base; //tick base time

#pragma mark tick
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*tick*/
#define yx_os_ticket_now() mach_absolute_time() //gettimeofday?

static yx_inline
yx_real yx_os_ticket_toNanoSecond(yx_os_tick tick){
    
    if (_yx_os_ticket_base < 0.0f) {
        mach_timebase_info_data_t info = {0, 0};
        mach_timebase_info(&info);
        _yx_os_ticket_base = info.numer / info.denom;
    }
    
    return tick * _yx_os_ticket_base;
}

#pragma mark time
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*time*/

//#define yx_os_time_now()                      (yx_os_time)time((time_t*)NULL)

static yx_inline
yx_os_time yx_os_time_now()
{
    return (yx_os_time)time((time_t*)NULL);
}

#define yx_os_time_fromDate(date_ptr)         (yx_os_time)mktime(date_ptr)


#pragma mark date
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*date*/

static yx_inline
yx_os_date* yx_os_date_local(yx_os_time time){
    time_t ctime = (time_t)time;
    return localtime(&ctime);
}

static yx_inline
yx_os_date* yx_os_date_UTC(yx_os_time time){
    time_t ctime = (time_t)time;
    return gmtime(&ctime);
}

static yx_inline
yx_os_date* yx_os_date_now(yx_bool isUTC){
    time_t now = time((time_t*)NULL);
    return (yx_true == isUTC)? (gmtime(&now)) : (localtime(&now));
}


#define yx_os_date_string(buf, buf_len, fmt, date)       strftime(buf, buf_len, fmt, date)


#define yx_os_date_second(date_ptr)          ((date_ptr)->tm_sec)   /* seconds after the minute [0-60] */
#define yx_os_date_minute(date_ptr)          ((date_ptr)->tm_min)   /* minutes after the hour [0-59] */
#define yx_os_date_hour(date_ptr)            ((date_ptr)->tm_hour)  /* hours since midnight [0-23] */
#define yx_os_date_month(date_ptr)           ((date_ptr)->tm_mon)   /* months since January [0-11] */
#define yx_os_date_year(date_ptr)            (1900 + (date_ptr)->tm_year)  /* years since 1900 */

#define yx_os_date_dayInMonth(date_ptr)      ((date_ptr)->tm_mday)  /* day of the month [1-31] */
#define yx_os_date_dayInWeek(date_ptr)       ((date_ptr)->tm_wday)  /* days since Sunday [0-6] */
#define yx_os_date_dayInYear(date_ptr)       ((date_ptr)->tm_yday)  /* days since January 1 [0-365] */


#pragma mark posix time struct

/*
 time_spec_ref 将带回当前系统时间，从UTC1970-1-1 0:0:0开始计时
 第二个参数time_zone，带回当前的时区信息，如果不需要刻意设置为0
 timezone结构描述如下：
 struct timezone
 {
 int tz_minuteswest;  //和格林威治 时间差了多少分钟
 int tz_dsttime;  //日光节约时间的状态（夏时制）
 };
 */
#define yx_os_timeval_now(timeval_ref) gettimeofday(timeval_ref, 0)

/*用于计算 timeval offset的。 offset 是 yx_real 类型 */
#define yx_os_timeval_offset(timeval_ref, offset) \
do{\
yx_os_timeval_now(timeval_ref); \
timeval_ref->tv_sec += (yx_int)offset; \
timeval_ref->tv_usec += yx_os_sec2usec(offset - 1); \
}while(0)



/*
 第一个参数，获取制定的时间
 1. CLOCK_REALTIME:系统实时时间,随系统实时时间改变而改变,即从UTC1970-1-1 0:0:0开始计时,如果系统时间被用户改成其他,则对应的时间相应改变
 2. CLOCK_REALTIME_COARSE：和CLOCK_REALTIME类似，但是执行速度快，精度低
 3. CLOCK_MONOTONIC:从系统启动这一刻起开始计时,不受系统时间被用户改变的影响
 4. CLOCK_MONOTONIC_COARSE ：和CLOCK_MONOTONIC类似，但是执行速度快，精度低
 5. CLOCK_BOOTTIME：和　　CLOCK_MONOTONIC 类似，但是包括了系统休眠的时间
 6. CLOCK_PROCESS_CPUTIME_ID:本进程到当前代码系统CPU花费的时间
 7. CLOCK_THREAD_CPUTIME_ID:本线程到当前代码系统CPU花费的时间
 
 第二个参数，带回第一个参数指定的时间
 */
#define yx_os_timespec_now(timespec_ref) clock_gettime(CLOCK_REALTIME, timespec_ref)


/*用于计算 timespec offset的。 offset 是 yx_real 类型 */
#define yx_os_timespec_offset(timespec_ref, offset) \
do{ \
yx_os_timespec_now(timespec_ref); \
(timespec_ref)->tv_sec += (int)offset; \
(timespec_ref)->tv_nsec += yx_os_sec2nsec(offset - 1); \
}while(0)




#endif









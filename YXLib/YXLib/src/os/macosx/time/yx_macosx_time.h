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



#endif










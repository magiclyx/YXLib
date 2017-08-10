//
//  yx_macosx_critime.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/13.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_CRITIME_H_
#define _YX_MACOSX_CRITIME_H_

#include "../yx_macosx_global.h"

#include "yx_macosx_time.h"

/*
 精确时间定义
 */
typedef struct yx_os_critime_msec{
    yx_os_second sec;
    yx_os_msecond msec;
}yx_os_critime_msec;

typedef struct yx_os_critime_usec{
    yx_os_second sec;
    yx_os_usecond usec;
}yx_os_critime_usec;

typedef struct yx_os_critime_nsec{
    yx_os_second sec;
    yx_os_nsecond nsec;
}yx_os_critime_nsec;



////////////////////////////////////////////////////////////////////////////////////////////////////
/* msec */
////////////////////////////////////////////////////////////////////////////////////////////////////
yx_forceinline
yx_os_critime_msec* yx_os_critime_msec_initWithSec(yx_os_critime_msec* cri_msec, yx_os_second sec)
{
    cri_msec->sec = sec;
    cri_msec->msec = 0;
    
    return cri_msec;
}

yx_forceinline
yx_os_critime_msec* yx_os_critime_msec_initWithMsec(yx_os_critime_msec* cri_msec, yx_os_msecond msec)
{
    cri_msec->sec = yx_os_msec2sec(msec);
    cri_msec->msec = (msec % YX_MSEC_PER_SEC);
    
    return cri_msec;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/* usec */
////////////////////////////////////////////////////////////////////////////////////////////////////
//:~ TODO

////////////////////////////////////////////////////////////////////////////////////////////////////
/* nsec */
////////////////////////////////////////////////////////////////////////////////////////////////////
//:~ TODO



#endif /* defined(_YX_MACOSX_CRITIME_H_) */

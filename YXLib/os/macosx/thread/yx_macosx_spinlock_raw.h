//
//  yx_macosx_yx_os_spinlock_raw.h
//  YXLib
//
//  Created by Yuxi Liu on 2017/8/13.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//
// http://locklessinc.com/articles/locks/

#ifndef _YX_MACOSX_yx_os_spinlock_raw_RAW_H_
#define _YX_MACOSX_yx_os_spinlock_raw_RAW_H_

#include "yx_macosx_spinlock_base.h"


typedef unsigned yx_os_spinlock_raw;

yx_forceinline void yx_os_rawspin_lock(yx_os_spinlock_raw *lock)
{
    while (1)
    {
        if (!yx_os_xchg_32(lock, YX_EBUSY)) return;
        
        while (*lock) cpu_relax();
    }
}

yx_forceinline void yx_os_rawspin_unlock(yx_os_spinlock_raw *lock)
{
    barrier();
    *lock = 0;
}

yx_forceinline int yx_os_rawspin_trylock(yx_os_spinlock_raw *lock)
{
    return yx_os_xchg_32(lock, YX_EBUSY);
}


#endif /* _YX_MACOSX_yx_os_spinlock_raw_RAW_H_ */


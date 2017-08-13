//
//  yx_macosx_thread_spinlock.h
//  YXLib
//
//  Created by Yuxi Liu on 2017/8/11.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

//考虑直接使用 OSSpinLock

#ifndef _YX_MACOSX_THREAD_SPINLOCK_H_
#define _YX_MACOSX_THREAD_SPINLOCK_H_


#include "../yx_macosx_global.h"

typedef OSSpinLock yx_os_thread_spinlock;


#endif /* yx_macosx_thread_spinlock_h */

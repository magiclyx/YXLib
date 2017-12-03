//
//  yx_macosx_spinlock.h
//  YXLib
//
//  Created by Yuxi Liu on 2017/8/13.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//
// http://locklessinc.com/articles/locks/

#ifndef _YX_MACOSX_SPINLOCK_H_
#define _YX_MACOSX_SPINLOCK_H_

#include "yx_macosx_spinlock_base.h"
#include "../atomic/yx_macosx_atomic.h"

typedef union yx_os_spinlock yx_os_spinlock;

union yx_os_spinlock
{
    yx_os_atomic_uint32 u;
    struct
    {
        yx_uint16 ticket;
        yx_os_atomic_uint16 users;
    } s;
};

#define YX_OS_SPINLOCK_INIT {0}

yx_forceinline void yx_os_spin_lock(yx_os_spinlock *t)
{
    yx_uint16 me = yx_atomic_add(&t->s.users, 1);
    while (t->s.ticket != me) cpu_relax();
}

yx_forceinline void yx_os_spin_unlock(yx_os_spinlock *t)
{
    barrier();
    t->s.ticket++;
}

yx_forceinline int yx_os_spin_trylock(yx_os_spinlock *t)
{
    yx_uint32 me = t->s.users;
    yx_uint16 menew = me + 1;
    yx_uint32 cmp = ((unsigned) me << 16) + me;
    yx_uint32 cmpnew = ((unsigned) menew << 16) + me;
    
    
    if (yx_atomic_cmpxchg(&t->u, &cmp, cmpnew)) return 0;
    
    return YX_EBUSY;
}

yx_forceinline int yx_os_spin_lockable(yx_os_spinlock *t)
{
    yx_os_spinlock u = *t;
    barrier();
    return (u.s.ticket == u.s.users);
}

#endif /* _YX_MACOSX_SPINLOCK_H_ */

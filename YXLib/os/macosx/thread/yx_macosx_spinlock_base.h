//
//  spinlock_base.h
//  YXLib
//
//  Created by Yuxi Liu on 2017/8/13.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

//http://locklessinc.com/articles/locks/

#ifndef _YX_MACOSX_SPINLOCK_BASE_H_
#define _YX_MACOSX_SPINLOCK_BASE_H_

#include "../yx_macosx_global.h"
#include "../basictypes/yx_macosx_types.h"

#define YX_EBUSY 1

/* Compile read-write barrier */
#define barrier() asm volatile("": : :"memory")

/* Pause instruction to prevent excess processor bus usage */
#define cpu_relax() asm volatile("pause\n": : :"memory")


/* Atomic exchange (of various sizes) */
yx_forceinline yx_ptr yx_os_xchg_ptr(void *ptr, yx_ptr x)
{
    __asm__ __volatile__("xchgq %0,%1"
                         :"=r" (x)
                         :"m" (*(volatile yx_uint64 *)ptr), "0" ((yx_uint64) x)
                         :"memory");
    
    return x;
}

yx_forceinline yx_uint64 yx_os_xchg_64(void *ptr, yx_uint64 x)
{
    __asm__ __volatile__("xchgq %0,%1"
                         :"=r" (x)
                         :"m" (*(volatile yx_uint64 *)ptr), "0" ((yx_uint64) x)
                         :"memory");
    
    return x;
}


yx_forceinline yx_uint32 yx_os_xchg_32(void *ptr, yx_uint32 x)
{
    __asm__ __volatile__("xchgl %0,%1"
                         :"=r" (x)
                         :"m" (*(volatile yx_uint32 *)ptr), "0" (x)
                         :"memory");
    
    return x;
}

yx_forceinline yx_uint16 yx_os_xchg_16(void *ptr, yx_uint16 x)
{
    __asm__ __volatile__("xchgw %0,%1"
                         :"=r" (x)
                         :"m" (*(volatile yx_uint16 *)ptr), "0" (x)
                         :"memory");
    
    return x;
}


yx_forceinline yx_uint8 yx_os_xchg_8(void *ptr, yx_uint8 x)
{
    __asm__ __volatile__("xchgb %0,%1"
                         :"=r" (x)
                         :"m" (*(volatile yx_uint8 *)ptr), "0" (x)
                         :"memory");
    
    return x;
}

/* Test and set a bit */
yx_forceinline yx_char yx_os_atomic_bitsetandtest(yx_ptr ptr, yx_int x)
{
    yx_char out;
    __asm__ __volatile__("lock; bts %2,%1\n"
                         "sbb %0,%0\n"
                         :"=r" (out), "=m" (*(volatile yx_uint64 *)ptr)
                         :"Ir" (x)
                         :"memory");
    
    return out;
}




#endif /* _YX_MACOSX_SPINLOCK_BASE_H_ */

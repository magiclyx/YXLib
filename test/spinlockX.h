//
//  spinlock.h
//  YXLib
//
//  Created by Yuxi Liu on 2017/8/13.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#ifndef spinlock_h
#define spinlock_h


/* Spin lock using xchg. Added backoff wait to avoid concurrent lock/unlock
 * operation.
 * Original code copied from http://locklessinc.com/articles/locks/
 */

/* Compile read-write barrier */
#define barrier() asm volatile("": : :"memory")

/* Pause instruction to prevent excess processor bus usage */
#define cpu_relax() asm volatile("pause\n": : :"memory")

static inline unsigned short xchg_8(void *ptr, unsigned char x)
{
    __asm__ __volatile__("xchgb %0,%1"
                         :"=r" (x)
                         :"m" (*(volatile unsigned char *)ptr), "0" (x)
                         :"memory");
    
    return x;
}

#define BUSY 1
typedef unsigned char spinlock;

#define SPINLOCK_INITIALIZER 0

static inline void spin_lock(spinlock *lock)
{
    int wait = 1;
    while (1) {
        if (!xchg_8(lock, BUSY)) return;
        
        // wait here is important to performance.
        for (int i = 0; i < wait; i++) {
            cpu_relax();
        }
        while (*lock) {
            wait *= 2; // exponential backoff if can't get lock
            for (int i = 0; i < wait; i++) {
                cpu_relax();
            }
        }
    }
}

static inline void spin_unlock(spinlock *lock)
{
    barrier();
    *lock = 0;
}

static inline int spin_trylock(spinlock *lock)
{
    return xchg_8(lock, BUSY);
}



#endif /* spinlock_h */

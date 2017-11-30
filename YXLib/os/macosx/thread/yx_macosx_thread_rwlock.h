//
//  yx_macosx_thread_rwlock.h
//  YXCLib
//
//  Created by Yuxi Liu on 2017/11/27.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_THREAD_RWLOCK_H_
#define _YX_MACOSX_THREAD_RWLOCK_H_

#include "../yx_macosx_global.h"

typedef struct{
    pthread_rwlock_t rwl;
}yx_os_thread_rwlock;

typedef yx_os_thread_rwlock* yx_os_thread_rwlock_ref;


#if YX_MUTTHREAD

/*declare*/
#define YX_OS_PTHREAD_RWLOCK_DECLARE(name) yx_os_thread_rwlock name
#define yx_os_pthread_rwlock_type_declare(name) yx_os_thread_rwlock name
#define yx_os_pthread_rwlock_ptr_declare(name) yx_os_thread_rwlock_ref name;

/*init & destroy*/
#define yx_os_pthread_rwlock_init(rwlock_ref) pthread_rwlock_init(&((rwlock_ref)->rwl), NULL)
#define yx_os_pthread_rwlock_recycle(rwlock_ref) pthread_rwlock_destroy(&((rwlock_ref)->rwl))

/*wlock*/
#define yx_os_pthread_rwlock_wlock(rwlock_ref) pthread_rwlock_wrlock(&((rwlock_ref)->rwl))
#define yx_os_pthread_rwlock_trywlock(rwlock_ref) pthread_rwlock_trywrlock(&(rwlock_ref->rwl))

/*rlock*/
#define yx_os_pthread_rwlock_rlock(rwlock_ref) pthread_rwlock_rdlock(&((rwlock_ref)->rwl))
#define yx_os_pthread_rwlock_tryrlock(rwlock_ref) pthread_rwlock_tryrdlock(&(rwlock_ref->rwl))

/*unlock*/
#define yx_os_pthread_rwlock_unlock(rwlock_ref) pthread_rwlock_unlock(&((rwlock_ref)->rwl))

#else

/*declare*/
#define YX_OS_PTHREAD_RWLOCK_DECLARE(name)
#define yx_os_pthread_rwlock_type_declare(name)
#define yx_os_pthread_rwlock_ptr_declare(name)

/*init & destroy*/
#define yx_os_pthread_rwlock_init(rwlock_ref)
#define yx_os_pthread_rwlock_recycle(rwlock_ref)

/*wlock*/
#define yx_os_pthread_rwlock_wlock(rwlock_ref)
#define yx_os_pthread_rwlock_trywlock(rwlock_ref)

/*rlock*/
#define yx_os_pthread_rwlock_rlock(rwlock_ref)
#define yx_os_pthread_rwlock_tryrlock(rwlock_ref)


#endif

#endif /* yx_macosx_thread_rwlock_h */

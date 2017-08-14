//
//  yx_macosx_thread_lock.h
//  YXLib
//
//  Created by Yuxi Liu on 2017/8/11.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_THREAD_LOCK_H_
#define _YX_MACOSX_THREAD_LOCK_H_

#include "../yx_macosx_global.h"

typedef struct{
    pthread_mutex_t mutex;
}yx_os_thread_mutex;

typedef yx_os_thread_mutex* yx_os_thread_mutex_ref;


#if YX_MUTTHREAD


#define YX_OS_PTHREAD_MUTEX_DECLARE(name) yx_os_thread_mutex name



#define yx_os_pthread_mutex_type_declare(name) \
    yx_os_thread_mutex name
#define yx_os_pthread_mutex_ptr_declare(name) \
    yx_os_thread_mutex_ref name

#define yx_os_pthread_mutex_static \
    PTHREAD_MUTEX_INITIALIZER

#define yx_os_pthread_mutex_init(mutex_ref) \
    pthread_mutex_init(&(mutex_ref->mutex), NULL)

#define yx_os_pthread_mutex_recycle(mutex_ref) \
    pthread_mutex_destroy(&(mutex_ref->mutex))

#define yx_os_pthread_mutex_lock(mutex_ref) \
    pthread_mutex_lock(&(mutex_ref->mutex))

#define yx_os_pthread_mutex_unlock(mutex_ref) \
    pthread_mutex_unlock(&(mutex_ref->mutex))

#define yx_os_pthread_mutex_trylock(mutex_ref) \
    pthread_mutex_trylock(&(mutex_ref->mutex))

#else

#define YX_OS_PTHREAD_MUTEX_DECLARE(name)


#define yx_os_pthread_mutex_type_declare(name)
#define yx_os_pthread_mutex_ptr_declare(name)

#define yx_os_pthread_mutex_static

#define yx_os_pthread_mutex_init(mutex_ref)

#define yx_os_pthread_mutex_recycle(mutex_ref)

#define yx_os_pthread_mutex_lock(mutex_ref)

#define yx_os_pthread_mutex_unlock(mutex_ref)

#define yx_os_pthread_mutex_trylock(mutex_ref)


#endif


#endif /* yx_macosx_thread_lock_h */

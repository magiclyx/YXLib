//
//  yx_macosx_thread_cond.h
//  YXCLib
//
//  Created by Yuxi Liu on 2017/11/29.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_THREAD_COND_H_
#define _YX_MACOSX_THREAD_COND_H_

#include "../yx_macosx_global.h"
#include "yx_macosx_thread_lock.h"
#include "../basictypes/yx_macosx_types.h"
#include "../time/yx_macosx_time.h"

typedef struct{
    pthread_cond_t cond;
}yx_os_thread_cond;

typedef yx_os_thread_cond* yx_os_thread_cond_ref;


#if YX_MUTTHREAD


/*declare*/
#define YX_OS_PTHREAD_COND_DECLARE(name) yx_os_thread_cond name
#define yx_os_pthread_cond_type_declare(name) yx_os_thread_cond name
#define yx_os_pthread_cond_ptr_declare(name) yx_os_thread_cond_ref name
//#define yx_os_pthread_cond_static_declare(name) PTHREAD_COND_INITIALIZER name


/*init & destroy*/
//只有在没有线程在该条件变量上等待的时候才能注销这个条件变量，否则返回EBUSY
#define yx_os_pthread_cond_init(cond_ref) pthread_cond_init(&((cond_ref)->cond), NULL)
#define yx_os_pthread_cond_recycle(cond_ref) pthread_cond_destroy(&((cond_ref)->cond))


/*wait*/
//无论哪种等待方式，都必须和一个互斥锁配合，以防止多个线程同时请求pthread_cond_wait().
//lock->wait->unlock
#define yx_os_pthread_cond_wait(cond_ref, mutex_ref) pthread_cond_wait(&((cond_ref)->cond), yx_os_pthread_mutex_mutexRef(mutex_ref))

yx_forceinline int yx_os_pthread_cond_timewait(yx_os_thread_cond_ref cond_ref, yx_os_thread_mutex_ref mutex_ref, yx_real time){
    yx_os_timespec timespec;
    yx_os_timespec_offset(&timespec, time);
    return pthread_cond_timedwait(&(cond_ref->cond), yx_os_pthread_mutex_mutexRef(mutex_ref), &timespec);
}

/*active*/

//激活一个
#define yx_os_pthread_cond_signal(cond_ref) pthread_cond_signal(&((cond_ref)->cond))

//激活所有
#define yx_os_pthread_cond_boarcast(cond_ref) pthread_cond_broadcast(&((cond_ref)->cond))




#else //-----------------------------------------------------------------------------



/*declare*/
#define YX_OS_PTHREAD_COND_DECLARE(name)
#define yx_os_pthread_cond_type_declare(name)
#define yx_os_pthread_cond_ptr_declare(name)
//#define yx_os_pthread_cond_static_declare(name) PTHREAD_COND_INITIALIZER name


/*init & destroy*/
//只有在没有线程在该条件变量上等待的时候才能注销这个条件变量，否则返回EBUSY
#define yx_os_pthread_init(cond_ref)
#define yx_os_pthread_recycle(cond_ref)


/*wait*/
//无论哪种等待方式，都必须和一个互斥锁配合，以防止多个线程同时请求pthread_cond_wait().
//lock->wait->unlock
#define yx_os_pthread_wait(cond_ref, mutex_ref)

#define yx_os_pthread_timewait(cond_ref, mutex_ref, time)


/*active*/

//激活一个
#define yx_os_pthread_signal(cond_ref)

//激活所有
#define yx_os_pthread_boarcast(cond_ref)


#endif



#endif /* _YX_MACOSX_THREAD_COND_H_ */

//
//  yx_macosx_thread_event.h
//  YXCLib
//
//  Created by Yuxi Liu on 2017/11/30.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_THREAD_EVENT_H_
#define _YX_MACOSX_THREAD_EVENT_H_

#include "yx_macosx_thread_cond.h"
#include "yx_macosx_thread_lock.h"

typedef struct{
    yx_os_thread_cond cond;
    yx_os_thread_mutex mutex;
}yx_os_thread_event;

typedef yx_os_thread_event* yx_os_thread_event_ref;

#if YX_MUTTHREAD

/*declare*/
#define YX_OS_PTHREAD_EVENT_DECLARE(name) yx_os_thread_event name
#define yx_os_pthread_event_type_declare(name) yx_os_thread_event name
#define yx_os_pthread_event_ptr_declare(name) yx_os_thread_event_ref name


/*init & destroy*/
#define yx_os_pthread_event_init(event_ref) \
  yx_os_pthread_mutex_init(&((event_ref)->mutex)) \
  yx_os_pthread_cond_init(&((event_ref)->cond))


#define yx_os_pthread_event_recycle(event_ref) \
  yx_os_pthread_cond_recycle(&((event_ref)->cond)) \
  yx_os_pthread_mutex_recycle(&((event_ref)->mutex)) \


/*wait*/
yx_forceinline
yx_int yx_os_pthread_event_wait(yx_os_thread_event_ref event_ref){
    yx_int rt;
    yx_os_pthread_mutex_lock(&((event_ref)->mutex));
    rt = yx_os_pthread_cond_wait(&((event_ref)->cond), &((event_ref)->mutex));
    yx_os_pthread_mutex_unlock(&((event_ref)->mutex));
    
    return rt;
}

yx_forceinline
yx_int yx_os_pthread_event_timewait(yx_os_thread_event_ref event_ref, yx_real time){
    yx_int rt;
    yx_os_pthread_mutex_lock(&((event_ref)->mutex));
    rt = yx_os_pthread_cond_timewait(&((event_ref)->cond), &((event_ref)->mutex), time);
    yx_os_pthread_mutex_unlock(&((event_ref)->mutex));
    
    return rt;
}



/*active*/
//激活一个
#define yx_os_pthread_event_signal(event_ref) yx_os_pthread_cond_signal(&((event_ref)->cond))
//激活所有
#define yx_os_pthread_event_boardcast(event_ref) yx_os_pthread_cond_signal(&((event_ref)->cond))


#else


/*declare*/
#define YX_OS_PTHREAD_EVENT_DECLARE(name)
#define yx_os_pthread_event_type_declare(name)
#define yx_os_pthread_event_ptr_declare(name)


/*init & destroy*/
#define yx_os_pthread_event_init(event_ref)
#define yx_os_pthread_event_recycle(event_ref)


/*wait*/
#define yx_os_pthread_event_wait(event_ref)

#define yx_os_pthread_event_timewait(event_ref time)



/*active*/
//激活一个
#define yx_os_pthread_event_signal(event_ref)
//激活所有
#define yx_os_pthread_event_boardcast(event_ref)



#endif


#endif /* _YX_MACOSX_THREAD_EVENT_H_ */

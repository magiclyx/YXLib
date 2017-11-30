//
//  yx_macosx_thread_sem.c
//  YXCLib
//
//  Created by Yuxi Liu on 2017/11/30.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_thread_sem.h"
#include "../yx_macosx_global.h"

typedef struct yx_os_thread_sem{
    sem_t sem;
}yx_os_thread_sem;


typedef struct yx_os_thread_sem* yx_os_thread_sem_ref;

#if YX_MUTTHREAD

/*declare*/
#define YX_OS_PTHREAD_SEM_DECLARE(name) yx_os_thread_sem name
#define yx_os_pthread_sem_type_declare(name) yx_os_thread_sem name
#define yx_os_pthread_sem_ptr_declare(name) yx_os_thread_sem_ref name


/* init & destroy */
//第二个参数，0表示县城内共享。 非0表示进程内共享
#define yx_os_pthread_sem_init(sem_ref, isShared, num) sem_init(&((sem_ref)->sem), ((isShared)? 1 : 0), num)
#define yx_os_pthread_sem_recycle(sem_ref) sem_destroy(&((sem_ref)->sem))


/* wait */
#define yx_os_pthread_sem_wait(sem_ref) sem_wait(&((sem_ref)->sem))
#define yx_os_pthread_sem_trywait(sem_ref) sem_trywait(&((sem_ref)->sem)) //如果失败，返回 EAGAIN

/* active */
#define yx_os_pthread_sem_post(sem_ref) sem_post(&((sem_ref)->sem))

/* value */
//val_ptr 返回value
#define yx_os_pthread_sem_value(sem_ref, val_ptr) sem_getvalue(&((sem_ref)->sem), val_ptr)

#else
#endif

//
//  yx_macosx_thread.h
//  YXLib
//
//  Created by LiuYuxi on 15/9/30.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_THREAD_H_
#define _YX_MACOSX_THREAD_H_


typedef struct yx_os_thread{
    pthread_t thread;
}yx_os_thread;

typedef struct yx_os_thread* yx_os_thread_ref;



///////////////////////////////////////////////////////
/*create*/
///////////////////////////////////////////////////////
typedef void*(*thread_entry_func)(void*);
#define yx_os_thread_create(thread_ref, fun_ptr, parg) pthread_create(&((thread_ref)->thread), NULL, fun_ptr, parg)


///////////////////////////////////////////////////////
/*detach*/
///////////////////////////////////////////////////////
#define yx_os_thread_detach(thread_ref) pthread_detach((thread_ref)->thread)

yx_forceinline yx_int yx_os_thread_create_detach(yx_os_thread_ref thread_ref, thread_entry_func func_ptr, void* parg){
    yx_int rt = yx_os_thread_create(thread_ref, func_ptr, parg);
    yx_os_thread_detach(thread_ref);
    
    return rt;
}

///////////////////////////////////////////////////////
/*property*/
///////////////////////////////////////////////////////

//macos 上thread_t 是结构体，不能直接打印pthread_t, 因此使用 pthread_mach_threa_np 获取
#define yx_os_thread_id(thread_ref) pthread_mach_thread_np((thread_ref)->thread)

//比较是否相等
#define yx_os_thread_equal(thread_ref_1, thread_ref_2) pthread_equal((thread_ref_1)->thread, (thread_ref_2)->thread)

//获取当前pthread
#define yx_os_thread_self() {pthread_self(void)}


///////////////////////////////////////////////////////
/*exit & join*/
///////////////////////////////////////////////////////
#define yx_os_thread_exit(exit_val_ptr) pthread_exit(exit_val_ptr) //exit_val_ptr 会返回给pthread_join
#define yx_os_thread_join(thread_ref, exit_val_ptr_ref) pthread_join((thread_ref)->thread, exit_val_ptr_ref)

///////////////////////////////////////////////////////
/*cancel*/
///////////////////////////////////////////////////////

//取消其他线程
#define yx_os_thread_cancel(thread_ref) pthread_cancel((thread_ref)->thread) 

//设置当前线程是否可以被取消(默认是YES)
yx_forceinline
yx_int yx_os_thread_setCancelEnable(yx_bool enable){ 
    int old_state;
    return pthread_setcancelstate(((yx_true == enable)? PTHREAD_CANCEL_ENABLE:PTHREAD_CANCEL_DISABLE), &old_state);
}

//添加一个新的取消点
#define yx_os_thread_setCancelPoint() pthread_testcancel(void)

//是否必须是取消点才可以取消(默认是YES)
yx_forceinline
yx_int yx_os_thread_mustCancelPoint(yx_bool mustOnCancelPoint){
    int old_type;
    
    return pthread_setcanceltype(((yx_true == mustOnCancelPoint)? PTHREAD_CANCEL_DEFERRED:PTHREAD_CANCEL_ASYNCHRONOUS), &old_type);
}


///////////////////////////////////////////////////////
/*cleanup fun*/
///////////////////////////////////////////////////////
typedef void(*thread_cleanup_func)(void*);

//用于添加清理函数
#define yx_os_thread_cleanup_push(cleanup_func_ptr, parg) pthread_cleanup_push(cleanup_func_ptr, parg)
#define yx_os_thread_cleanup_pop(exit_code) pthread_cleanup_pop(exit_code)

///////////////////////////////////////////////////////
/*sleep*/
///////////////////////////////////////////////////////

#define yx_os_thread_sleep(time)   sleep(time)


///////////////////////////////////////////////////////
/*once*/
///////////////////////////////////////////////////////
typedef void(*thread_initonce_func)(void);

typedef  pthread_once_t yx_os_thread_once_flag;
#define YX_OS_THREAD_ONCE_DECLARE(name) yx_os_thread_once_flag name = PTHREAD_ONCE_INIT

#define yx_os_thread_runonce(once_flag_ref, once_func) pthread_once(once_flag_ref, once_func)


///////////////////////////////////////////////////////
/*thread associate value*/
///////////////////////////////////////////////////////

typedef void(*thread_associate_destructor_func)(void*);

typedef pthread_key_t yx_os_thread_key;

#define yx_os_thread_associatekey_create(key_ref, destructor_func) pthread_key_create(key_ref, destructor_func)
#define yx_os_thread_assockatekey_delete(key_ref) pthread_key_delete(key_ref)

#define yx_os_thread_assockatekey_get(key) pthread_getspecific(key)
#define yx_os_thread_associatekey_set(key, value) pthread_setspecific(key, value)

#endif /* _YX_MACOSX_THREAD_H_ */




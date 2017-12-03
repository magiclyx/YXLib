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

typedef void*(*thread_entry_func)(void*);
typedef void(*thread_cleanup_func)(void*);


/*create*/
#define yx_os_thread_create(thread_ref, fun_ptr, parg) pthread_create(&((thread_ref)->thread), NULL, fun_ptr, parg)

#define yx_os_thread_detach(thread_ref) pthread_detach((thread_ref)->thread)

yx_forceinline yx_int yx_os_thread_create_detach(yx_os_thread_ref thread_ref, thread_entry_func func_ptr, void* parg){
    yx_int rt = yx_os_thread_create(thread_ref, func_ptr, parg);
    yx_os_thread_detach(thread_ref);
    
    return rt;
}

/*property*/

//macos 上thread_t 是结构体，不能直接打印pthread_t, 因此使用 pthread_mach_threa_np 获取
#define yx_os_thread_id(thread_ref) pthread_mach_thread_np((thread_ref)->thread)

//比较是否相等
#define yx_os_thread_equal(thread_ref_1, thread_ref_2) pthread_equal((thread_ref_1)->thread, (thread_ref_2)->thread)

//获取当前pthread
#define yx_os_thread_self() {pthread_self(void)}


/*exit*/
#define yx_os_thread_exit(exit_val_ptr) pthread_exit(exit_val_ptr) //exit_val_ptr 会返回给pthread_join
#define yx_os_thread_join(thread_ref, exit_val_ptr_ref) pthread_join((thread_ref)->thread, exit_val_ptr_ref)

#define yx_os_thread_cancel(thread_ref) pthread_cancel((thread_ref)->thread) //取消其他线程

/*cleanup fun*/
//用于添加清理函数
#define yx_os_thread_cleanup_push(cleanup_func_ptr, parg) pthread_cleanup_push(cleanup_func_ptr, parg)
#define yx_os_thread_cleanup_pop(exit_code) pthread_cleanup_pop(exit_code)

/*sleep*/
#define yx_os_thread_sleep(time)   sleep(time)


#endif /* _YX_MACOSX_THREAD_H_ */




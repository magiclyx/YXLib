//
//  yx_core_childprocmanager.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/12.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_CHILDPROCMANAGER_H_
#define _YX_CORE_CHILDPROCMANAGER_H_

#include "yx_core_process.h"

#include "../yx_core_global.h"
#include "../container/yx_core_list.h"

/*
 主要用于搜索，查找等管理
 监控进程关闭等问题
 */

typedef struct yx_core_childprocmanager{
    yx_core_list proc_list;
    
    yx_allocator allocator;
    yx_allocator proc_allocator;  /*用于申请yx_core_process结构*/
    
}yx_core_childprocmanager;

typedef struct yx_core_childprocmanager* yx_core_childprocmanager_ref;



yx_bool yx_core_childprocmanager_init(yx_allocator allocator, yx_core_childprocmanager_ref manager);
void yx_core_childprocmanager_recycle(yx_core_childprocmanager_ref manager);


yx_bool yx_core_childprocmanager_forkchildproc(yx_core_childprocmanager manager, yx_core_process_ref process, const yx_char* name, yx_bool* isChild);


#endif /* defined(_YX_CORE_CHILDPROCMANAGER_H_) */










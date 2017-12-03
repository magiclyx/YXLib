//
//  yx_core_childprocmanager.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/12.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_childprocmanager.h"
#include "../rttidef/yx_core_rttidef.h"
#include "../allocator/yx_core_membucket.h"



yx_bool yx_core_childprocmanager_init(yx_allocator allocator, yx_core_childprocmanager_ref manager)
{
    assert(NULL != manager);
    assert(NULL != allocator);
    
    yx_core_list_init(allocator, &(manager->proc_list));
    
    manager->allocator = allocator;
    /*setup the node allocator*/
    yx_os_rtti_if(allocator, yx_rtti_allocator_memsection) {
        /*因为是bufpool, 所以这个cellMempool不会被释放*/
        manager->proc_allocator = yx_membucket_create(allocator, sizeof(struct yx_core_listnode_wrapper));
    }
    else
    {
        manager->proc_allocator = allocator;
    }

    
    
    return yx_true;
}

void yx_core_childprocmanager_recycle(yx_core_childprocmanager_ref manager)
{
    assert(NULL != manager);
    
    yx_core_list_recycle(&(manager->proc_list));
}


//
//yx_bool yx_core_childprocmanager_forkchildproc(yx_core_childprocmanager manager, yx_core_process_ref process, const yx_char* name, yx_bool* isChild)
//{
//    return yx_core_process_fork(process, name, isChild);
//}





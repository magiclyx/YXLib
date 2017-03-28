//
//  yx_core_moduleManager.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/13.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_moduleManager.h"
#include "../debug/yx_core_assert.h"
#include "yx_core_rootmodule.h"
#include "yx_core_container_valfunc.h"


yx_bool yx_core_moduleManager_init(yx_allocator allocator, yx_core_moduleManager_ref manager)
{
    YX_ASSERT(NULL != allocator);
    YX_ASSERT(NULL != manager);
    
    manager->allocator = allocator;
    
    yx_core_rbtree_init(allocator, &(manager->moduleDict));
    yx_core_rbtree_set_callback_hash(&(manager->moduleDict), YX_CORE_HASHFUN_STRING);
    yx_core_rbtree_set_callback_compare(&(manager->moduleDict), YX_CORE_COMFUN_STRING);
    
    yx_os_memzero(manager->moduleArray, sizeof(yx_core_module_ref) * YX_MODULE_MAX);
    manager->rootModule = &yx_core_rootModule;
    manager->moduleCount = 1;
    manager->staticModuleIndex = 1;
    YX_ASSERT(YX_MODULE_MAX >= 1);
    
    
    return yx_true;
}

yx_bool yx_core_moduleManager_recycle(yx_core_moduleManager_ref* manager_ref)
{
    yx_core_moduleManager_ref moduleManager = *manager_ref;
    *manager_ref = NULL;
    
    yx_core_rbtree_recycle(&(moduleManager->moduleDict));
    
    return yx_true;
}

yx_bool yx_core_moduleManager_registModule(yx_core_moduleManager_ref manager, yx_core_module_ref module, yx_bool isDynamic)
{
    
    /*
     插入 array
     
    策略是:
     每次插入动态module都会从末尾开始遍历一遍，找到NULL的位置
     每次插入静态module，就按照位置向前查找
     */
    if (yx_true == isDynamic)
    {
        yx_uint newIndex = YX_MAX_UINT;
        
        for (yx_uint index = YX_MODULE_MAX - 1; index > manager->staticModuleIndex; index--)
        {
            if (NULL == manager->moduleArray[index])
            {
                newIndex = index;
                break;
            }
        }
        
        
        if (YX_MAX_UINT != newIndex)
        {
            manager->moduleArray[newIndex] = module;
        }
        else
        {
            YX_ASSERT(0);
            return yx_false;
        }
    }
    else
    {
        if (NULL == manager->moduleArray[manager->staticModuleIndex])
        {
            manager->moduleArray[manager->staticModuleIndex] = module;
            manager->staticModuleIndex++;
        }
        else
        {
            YX_ASSERT(0);
            return yx_false;
        }
    }
    
    
    /* 插入dict */
    yx_core_rbtree_setDataForKey(&(manager->moduleDict), (yx_value)module, (yx_value)(module->module_name));
    
    
    return yx_true;
}






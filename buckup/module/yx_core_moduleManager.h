//
//  yx_core_moduleManager.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/13.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef yx_core_moduleManager_h
#define yx_core_moduleManager_h

#include "../yx_core_global.h"
#include "yx_core_module.h"
#include "../container/yx_core_rbtree.h"

typedef struct yx_core_moduleManager{
    yx_allocator allocator;
    
    yx_core_module_ref rootModule;
    
    yx_core_module_ref moduleArray[YX_MODULE_MAX];
    yx_uint staticModuleIndex;
    yx_uint moduleCount;
    
    struct yx_core_rbtree moduleDict;
    
}yx_core_moduleManager, *yx_core_moduleManager_ref;


yx_bool yx_core_moduleManager_init(yx_allocator allocator, yx_core_moduleManager_ref manager);
yx_bool yx_core_moduleManager_recycle(yx_core_moduleManager_ref* manager_ref);

yx_bool yx_core_moduleManager_registModule(yx_core_moduleManager_ref manager, yx_core_module_ref module, yx_bool isDynamic);

#endif /* yx_core_moduleManager_h */













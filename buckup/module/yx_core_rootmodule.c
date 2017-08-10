//
//  yx_core_rootmodule.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/10.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_rootmodule.h"


yx_core_module yx_core_rootModule =
{
    "root_module", //module_name
    
    YX_CORE_LISTNODE_SENTINEL(yx_core_rootModule.sub_modules), //node
    
    { //sub_modules
        NULL,
        NULL,
    },
    
    NULL, //parent_module
    
    NULL, //context
    
    NULL, //_core_module_ask_unload
    NULL, //_core_module_did_load
    NULL, //_core_module_will_unload
    NULL, //_core_module_release
};


yx_bool yx_core_rootmodule_init(void)
{
    return yx_true;
}
yx_bool yx_core_rootmodule_recycle(void)
{
    return yx_true;
}








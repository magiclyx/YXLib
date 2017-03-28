//
//  yx_core_module.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/10.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_module.h"
#include "../debug/yx_core_assert.h"

yx_bool yx_core_module_init(yx_core_module_ref module, const yx_char* module_name)
{
    YX_ASSERT(NULL != module);
    yx_core_listnode_sentinelInit(&(module->sub_modules));
    
    module->module_name = module_name;
    
    
    module->parent_module = NULL;
    module->context = NULL;
    
    module->_core_module_ask_unload = NULL;
    module->_core_module_did_load = NULL;
    module->_core_module_will_unload = NULL;
    module->_core_module_release = NULL;

    return yx_true;
}



/*这里使用递归，是因为module数量不会太多*/
void yx_core_module_run_each_from_parent(yx_core_module_ref module, yx_value data, yx_bool (*run_module)(yx_core_module_ref module , yx_value data))
{
    struct yx_core_module* cursor;
    yx_core_listnode_ownerForEach(cursor, &(module->sub_modules), node)
    {
        if (yx_true == run_module(cursor, data))
        {
            yx_core_module_run_each_from_parent(cursor, data, run_module);
        }
    }
}

void yx_core_module_run_each_from_child(yx_core_module_ref module, yx_value data, yx_bool (*run_module)(yx_core_module_ref module , yx_value data))
{
    struct yx_core_module* cursor;
    yx_core_listnode_ownerForEach(cursor, &(module->sub_modules), node)
    {
        
        yx_core_module_run_each_from_parent(cursor, data, run_module);
        
        if (yx_true != run_module(cursor, data))
        {
            break;
        }
    }

}



void yx_core_module_addSubModule(yx_core_module_ref module, yx_core_module_ref subModule)
{
    yx_core_listnode_insertTail(&(module->sub_modules), &(subModule->node));
    subModule->parent_module = module;
}


void yx_core_module_removeFromSuperModule(yx_core_module_ref module)
{
    yx_core_listnode_del(&(module->node));
    module->parent_module = NULL;
    
#if YX_DEBUG
    module->node.next = NULL;
    module->node.prev = NULL;
#endif
}

















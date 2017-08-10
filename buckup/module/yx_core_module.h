//
//  yx_core_module.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/10.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef yx_core_module_h
#define yx_core_module_h

#include "../container/yx_core_listnode.h"


/*
 module 数量不要太多， 加载卸载不要太频繁
 */

typedef struct yx_core_module* yx_core_module_ref;

typedef struct yx_core_module{
    
    const yx_char* module_name;
    
    struct yx_core_listnode node;
    
    struct yx_core_listnode sub_modules;
    struct yx_core_module* parent_module;
    
    
    void* context;
    
    
    yx_bool (*_core_module_ask_unload)(); //询问是否可关闭
    void (*_core_module_did_load)();  //已经加载
    void (*_core_module_will_unload)(); //将要关闭
    
    yx_bool (*_core_module_release); //销毁模块

}yx_core_module;


extern yx_core_module yx_core_rootModule;


#define yx_core_module_parent(module)   ((module)->parent_modules)
#define yx_core_module_children(module) ((module)->sub_modules)
#define yx_core_module_context(module)  ((module)->context)

yx_bool yx_core_module_init(yx_core_module_ref module, const yx_char* module_name);

/*回调函数, 如果返回false, 停止当前结点其他分支的遍历*/
void yx_core_module_run_each_from_parent(yx_core_module_ref module, yx_value data, yx_bool (*run_module)(yx_core_module_ref module , yx_value data));
void yx_core_module_run_each_from_child(yx_core_module_ref module, yx_value data, yx_bool (*run_module)(yx_core_module_ref module , yx_value data));


void yx_core_module_addSubModule(yx_core_module_ref module, yx_core_module_ref subModule);
void yx_core_module_removeFromSuperModule(yx_core_module_ref module);


#endif /* yx_core_module_h */








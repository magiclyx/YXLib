//
//  yx_core_logger_rule.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/9.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_LOGGER_RULE_H_
#define _YX_CORE_LOGGER_RULE_H_

#include "../yx_core_global.h"
#include "yx_core_logger_def.h"

typedef void (*yx_log_parser_func)(yx_os_event_info_ref event_info);


typedef struct yx_core_logrule{
    yx_char* log_fmt;    /*日志格式*/
    yx_char* time_fmt;   /*时间格式*/
    yx_log_parser_func* func_array;  /*日志操作函数列表*/
    yx_char* newline_flag; /*换行符*/
    yx_size newline_flag_len; /*换行符长度*/
    
    yx_uint retain_count;  /*引用计数器*/
    
    yx_bool isUTC;  /*是否使用UTC时间*/
}yx_core_logrule, *yx_core_logrule_ref;


yx_core_logrule_ref yx_core_logrule_create(yx_allocator allocator);
yx_core_logrule_ref yx_core_logrule_createByConfigDict(yx_allocator allocator, void* configDict);
yx_core_logrule_ref yx_core_logrule_createByReference(yx_allocator allocator, yx_core_logrule_ref logrule);
yx_bool yx_core_logrule_release(yx_core_logrule_ref* logrule_ref);

typedef struct yx_core_logrule_manager{
    yx_allocator allocator;
    yx_core_logrule rule[YX_CORE_LOGGER_LOCKED_LEVEL+1]; /*这里使用锁定的等级来初始化rule等级*/
}yx_core_logrule_manager, *yx_core_logrule_manager_ref;


#define yx_core_logrule_getrul(manager, level) ((manager)->rule[level])


yx_bool yx_core_logrule_manager_init(yx_allocator allocator, yx_core_logrule_manager_ref manager);
yx_bool yx_core_logrule_manager_initWithDefaultRule(yx_allocator allocator, yx_core_logrule_manager_ref manager, yx_core_logrule_ref logrule);


typedef struct yx_log_rule
{
    yx_char logfmt[YX_CORE_LOGGER_FORMAT_MAX_LEN+1];
    yx_char timefmt[YX_CORE_LOGGER_FORMAT_MAX_LEN+1];
    yx_log_parser_func funcArr[YX_CORE_LOGGER_FORMAT_MAX_LEN+1];
    
    yx_char newline_flag[YX_CORE_LOGGER_NEWLINE_MAX_LEN+1]; //换行符s
    yx_size newline_len; //换行符长度
    
    yx_bool isUTC;
}yx_log_rule, *yx_log_rule_ref;



#endif /* _YX_CORE_LOGGER_RULE_H_ */






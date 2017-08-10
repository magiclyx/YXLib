//
//  yx_core_logger_rule.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/9.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_logger_rule.h"
#include "../debug/yx_core_assert.h"

#define yx_core_logrule_default_logfmt "[%d][%V][%p][%F:%L][%f]:%m%n"
#define yx_core_logrule_default_tmefmt "%Y-%m-%d %H:%M:%S"
#define yx_core_logrule_default_isUTC  yx_false


yx_core_logrule_ref yx_core_logrule_create(yx_allocator allocator)
{
    yx_core_logrule_ref logrule = NULL;
    yx_char* log_fmt = NULL;
    yx_char* time_fmt = NULL;
    yx_char* newline_flag = NULL;
    yx_size len;
    yx_size newline_flag_len;
    
    
    logrule = yx_allocator_alloc(allocator, sizeof(struct yx_core_logrule));
    if (yx_unlikely(NULL == logrule))
        goto errout;
    
    
    len = yx_os_strlen(yx_core_logrule_default_logfmt) + 1;
    log_fmt =  yx_allocator_alloc(allocator, sizeof(yx_char) * len);
    if (yx_unlikely(NULL == log_fmt))
        goto errout;

    
    
    len = yx_os_strlen(yx_core_logrule_default_tmefmt) + 1;
    time_fmt =  yx_allocator_alloc(allocator, sizeof(yx_char) * len);
    if (yx_unlikely(NULL == time_fmt))
        goto errout;
    
    
    newline_flag_len = yx_os_strlen(YX_NEWLINE) + 1;
    newline_flag  = yx_allocator_alloc(allocator, sizeof(yx_char) * newline_flag_len);
    if (yx_unlikely(NULL == newline_flag))
        goto errout;

    
    
    logrule->log_fmt = log_fmt;
    logrule->time_fmt = time_fmt;
    logrule->newline_flag = newline_flag;
    logrule->newline_flag_len = newline_flag_len;
    logrule->retain_count = 1;
    logrule->isUTC = yx_core_logrule_default_isUTC;
    logrule->func_array = NULL;
    
    return logrule;
    
errout:
    if (NULL != logrule)
        yx_allocator_free(allocator, logrule);
    
    if (NULL != log_fmt)
        yx_allocator_free(allocator, log_fmt);
    
    if (NULL != log_fmt)
        yx_allocator_free(allocator, time_fmt);
    
    if (NULL != log_fmt)
        yx_allocator_free(allocator, newline_flag);

    
    return NULL;
}

//yx_core_logrule_ref yx_core_logrule_createByConfigDict(yx_allocator allocator, void* configDict)
//{
//    return NULL;
//}
//
//yx_core_logrule_ref yx_core_logrule_createByReference(yx_allocator allocator, yx_core_logrule_ref logrule)
//{
//}
//
//yx_bool yx_core_logrule_release(yx_core_logrule_ref* logrule_ref)
//{
//}
//
//


yx_bool yx_core_logrule_manager_init(yx_allocator allocator, yx_core_logrule_manager_ref manager)
{
    YX_ASSERT(NULL != allocator);
    YX_ASSERT(NULL != manager);
    
    manager->allocator = allocator;
    
    
    return yx_true;
}

yx_bool yx_core_logrule_manager_initWithDefaultRule(yx_allocator allocator, yx_core_logrule_manager_ref manager, yx_core_logrule_ref logrule)
{
    YX_ASSERT(NULL != allocator);
    YX_ASSERT(NULL != manager);
    YX_ASSERT(NULL != logrule);
    
    manager->allocator = allocator;
    
    
    return yx_false;
}























































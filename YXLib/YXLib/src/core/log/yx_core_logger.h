//
//  yx_core_logger.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/5.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef yx_core_logger_h
#define yx_core_logger_h

#include "yx_core_global.h"
#include "yx_core_logger_def.h"
#include "yx_core_logger_rule.h"




//%d 日期
//%V level
//%p process ID
//%f function name
//%F file
//%L line
//%m message
//%n [new line]

//newlinecharacter=CRLF

typedef struct yx_core_logger* yx_core_logger_ref;

typedef struct yx_core_log_context{
    
    yx_core_logger_ref logger;
    
    const yx_char* file; //文件
    yx_size file_len;
    
    const yx_char* func; //函数
    yx_size func_len;
    
    yx_long line; //行数
    
    yx_logger_level level; //level
    
    
    const yx_char* format_string; //msg fmt
    yx_valist valist; //msg valist
    
    yx_os_date* date; //日期
    
}yx_core_log_context, *yx_core_log_context_ref;




typedef struct yx_core_logger{
    yx_allocator allocator;
    yx_char category_name[YX_OS_MAXPATH_LEN+1];
    yx_log_rule rule;
    yx_logger_level limited_level;
    yx_value data;
    
    yx_bool(*output_log)(struct yx_core_logger* logger, const yx_char* file, const yx_char* func, yx_long line, yx_logger_level level, const yx_char* format, ...);
}yx_core_logger, *yx_core_logger_ref;
    

yx_bool yx_core_logger_init(yx_allocator allocator, yx_core_logger_ref logger, const yx_char* yx_restrict name, yx_logger_level limited_level, void* rule_config, yx_value data);

void yx_core_logger_recycle(yx_core_logger_ref logger);


void yx_core_logger_build(yx_core_logger_ref logger);


void yx_core_log(yx_core_logger_ref logger,
                 const yx_char *file,
                 yx_size file_len,
                 const yx_char *func,
                 yx_size func_len,
                 long line,
                 const yx_logger_level level,
                 yx_vaparam(const yx_char* yx_restrict format)) yx_vafun_declare yx_printf_fmtcheck(8, 9);









#endif /* yx_core_logger_h */

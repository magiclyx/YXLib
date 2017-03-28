//
//  yx_core_logger_def.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/8.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_LOGGER_DEF_H_
#define _YX_CORE_LOGGER_DEF_H_

#include "../yx_core_global.h"


#define YX_CORE_LOGGER_FORMAT_MAX_LEN 63  /*日志和时间fmt字符串最大长度*/
#define YX_CORE_LOGGER_PARSER_FUN_MAX 33  /*保存解析函数指针的最大长度，一搬是fmt最大长度的一半*/
#define YX_CORE_LOGGER_NEWLINE_MAX_LEN 7  /*允许换行符最大长度*/

#define YX_CORE_LOGGER_LEVEL_COUNT 12 /*+ OFF & ALL*/


typedef enum yx_logger_level{
    YX_LOGGER_OFF = 0,
    YX_LOGGER_FATAL = 1,   /*致命错误*/
    YX_LOGGER_ALERT = 2,   /*报警*/
    YX_LOGGER_CRIT = 3,    /*临界警报 criticality*/
    YX_LOGGER_ERROR = 4,   /*可修复错误*/
    YX_LOGGER_WARN = 5,    /*运行警告*/
    YX_LOGGER_PERFORM = 6, /*性能警告*/
    YX_LOGGER_NOTICE = 7,  /*提示信息*/
    YX_LOGGER_INFO = 8,    /*运行信息*/
    YX_LOGGER_DEBUG = 9,   /*调试信息*/
    YX_LOGGER_TRACE = 10,  /*运行路径*/
    YX_LOGGER_ALL = 11,
}yx_logger_level;


/*大于这个等级的level都不会被编译*/
#define YX_CORE_LOGGER_LOCKED_LEVEL YX_LOGGER_ALL





#define YX_LOGGER_LEVEL_OFF_STR "OFF"
#define YX_LOGGER_LEVEL_OFF_STRLEN 3

#define YX_LOGGER_LEVEL_FATAL_STR "FATAL"
#define YX_LOGGER_LEVEL_FATAL_STRLEN 5

#define YX_LOGGER_LEVEL_ALERT_STR "ALERT"
#define YX_LOGGER_LEVEL_ALERT_STRLEN 5

#define YX_LOGGER_LEVEL_CRIT_STR "CRIT"
#define YX_LOGGER_LEVEL_CRIT_STRLEN 4

#define YX_LOGGER_LEVEL_ERROR_STR "ERROR"
#define YX_LOGGER_LEVEL_ERROR_STRLEN 5

#define YX_LOGGER_LEVEL_WARN_STR "WARN"
#define YX_LOGGER_LEVEL_WARN_STRLEN 4

#define YX_LOGGER_LEVEL_PERFORM_STR "PERFORM"
#define YX_LOGGER_LEVEL_PERFORM_STRLEN 7

#define YX_LOGGER_LEVEL_NOTICE_STR "NOTICE"
#define YX_LOGGER_LEVEL_NOTICE_STRLEN 6

#define YX_LOGGER_LEVEL_INFO_STR "INFO"
#define YX_LOGGER_LEVEL_INFO_STRLEN 4

#define YX_LOGGER_LEVEL_DEBUG_STR "DEBUG"
#define YX_LOGGER_LEVEL_DEBUG_STRLEN 5

#define YX_LOGGER_LEVEL_TRACE_STR "TRACE"
#define YX_LOGGER_LEVEL_TRACE_STRLEN 5

#define YX_LOGGER_LEVEL_ALL_STR "ALL"
#define YX_LOGGER_LEVEL_ALL_STRLEN 3

#define YX_LOGGER_LEVEL_UNKNOWN_STR "UNKNOWN"
#define YX_LOGGER_LEVEL_UNKNOWN_STRLEN 7



#endif /* _YX_CORE_LOGGER_DEF_H_ */




//
//  yx_core_logger.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/5.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_logger.h"
#include "../debug/yx_core_assert.h"

#include <asl.h>

#define YX_CORE_LOG_MAXBUFF_LEN  10240



static const yx_char* const YX_CORE_LEVEL_STR_MAP[] = {
    YX_LOGGER_LEVEL_OFF_STR,
    YX_LOGGER_LEVEL_FATAL_STR,
    YX_LOGGER_LEVEL_ALERT_STR,
    YX_LOGGER_LEVEL_CRIT_STR,
    YX_LOGGER_LEVEL_ERROR_STR,
    YX_LOGGER_LEVEL_WARN_STR,
    YX_LOGGER_LEVEL_PERFORM_STR,
    YX_LOGGER_LEVEL_NOTICE_STR,
    YX_LOGGER_LEVEL_INFO_STR,
    YX_LOGGER_LEVEL_DEBUG_STR,
    YX_LOGGER_LEVEL_TRACE_STR,
    YX_LOGGER_LEVEL_ALL_STR,
};


static const yx_int YX_CORE_LEVEL_STRLEN_MAP[] = {
    YX_LOGGER_LEVEL_OFF_STRLEN,
    YX_LOGGER_LEVEL_FATAL_STRLEN,
    YX_LOGGER_LEVEL_ALERT_STRLEN,
    YX_LOGGER_LEVEL_CRIT_STRLEN,
    YX_LOGGER_LEVEL_ERROR_STRLEN,
    YX_LOGGER_LEVEL_WARN_STRLEN,
    YX_LOGGER_LEVEL_PERFORM_STRLEN,
    YX_LOGGER_LEVEL_NOTICE_STRLEN,
    YX_LOGGER_LEVEL_INFO_STRLEN,
    YX_LOGGER_LEVEL_DEBUG_STRLEN,
    YX_LOGGER_LEVEL_TRACE_STRLEN,
    YX_LOGGER_LEVEL_ALL_STRLEN,
};






static yx_size _yx_core_logger_package_msg(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen);
static yx_size _yx_core_logger_package_date(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen);
static yx_size _yx_core_logger_package_file(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen);
static yx_size _yx_core_logger_package_func(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen);
static yx_size _yx_core_logger_package_line(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen);
static yx_size _yx_core_logger_package_newline(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen);
static yx_size _yx_core_logger_package_level(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen);
static yx_size _yx_core_logger_package_pid(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen);


yx_bool yx_core_logger_init(yx_allocator allocator, yx_core_logger_ref logger, const yx_char* yx_restrict name, yx_logger_level limited_level, void* rule_config, yx_value data)
{
    YX_ASSERT(NULL != allocator);
    YX_ASSERT(NULL != logger);
    YX_ASSERT(NULL != name);
    
    
    logger->allocator = allocator;
    
    yx_os_strncpy(logger->category_name, name, YX_OS_MAXPATH_LEN);
    logger->category_name[YX_OS_MAXPATH_LEN] = '\0';
    
    if (NULL != rule_config)
    {
        YX_ASSERT(0);
    }
    else
    {
        /*默认配置*/
        
        //%d 日期
        //%V level
        //%p process ID
        //%f function name
        //%F file
        //%L line
        //%m message
        //%n [new line]

        yx_os_strncpy(logger->rule.logfmt, "[%d][%V][%p][%F:%L][%f]:%m%n", YX_CORE_LOGGER_FORMAT_MAX_LEN);
        logger->rule.logfmt[YX_CORE_LOGGER_FORMAT_MAX_LEN] = '\0';
        
        logger->rule.isUTC = yx_false;
        yx_os_strncpy(logger->rule.timefmt, "%Y-%m-%d %H:%M:%S", YX_CORE_LOGGER_FORMAT_MAX_LEN);
        logger->rule.timefmt[YX_CORE_LOGGER_FORMAT_MAX_LEN] = '\0';
        
        yx_os_strncpy(logger->rule.newline_flag, "\n", YX_CORE_LOGGER_NEWLINE_MAX_LEN);
        logger->rule.newline_flag[YX_CORE_LOGGER_NEWLINE_MAX_LEN] = '\0';
        logger->rule.newline_len = 1;
    }
    
    logger->limited_level = limited_level;
    logger->data = data;
    
    return yx_true;
}

void yx_core_logger_recycle(yx_core_logger_ref logger)
{
#if YX_DEBUG
    
    logger->allocator = NULL;
    logger->category_name[0] = '\0';
    logger->limited_level = 99;
    logger->data = 0;
    
    
    logger->rule.logfmt[0] = '\0';
    logger->rule.isUTC = yx_false;
    logger->rule.timefmt[0] = '\0';
    logger->rule.newline_flag[0] = '\0';
    logger->rule.newline_len = 0;
    
#endif
}


void yx_core_logger_build(yx_core_logger_ref logger)
{
    YX_ASSERT(NULL != logger);
    
//    yx_char* fmt = logger->rule.logfmt;
}



void yx_core_log(yx_core_logger_ref logger,
                 const yx_char *file,
                 yx_size file_len,
                 const yx_char *func,
                 yx_size func_len,
                 long line,
                 const yx_logger_level level,
                 yx_vaparam(const yx_char* yx_restrict format)) yx_vafun_declare
{
    
    YX_ASSERT(NULL != logger);
    
    yx_core_log_context log_context;
    
    log_context.logger = logger;
    log_context.date = yx_os_date_now(logger->rule.isUTC);
    log_context.file = file;
    log_context.file_len = file_len;
    log_context.func = func;
    log_context.func_len = func_len;
    log_context.line = line;
    log_context.level = level;
    
    
    yx_char buff[YX_CORE_LOG_MAXBUFF_LEN + 1];
    yx_char* pcursor = buff;
    yx_size buf_len = YX_CORE_LOG_MAXBUFF_LEN;
    
    yx_size written_len;
    
    yx_vastart(log_context.valist, format);
    log_context.format_string = yx_vaarg_first(valist, const yx_char*, format);
    
    
    /*date*/
    written_len = _yx_core_logger_package_date(&log_context, pcursor, buf_len);
    if (written_len > 0)
    {
        pcursor += written_len;
        buf_len -= written_len;
    }

    /*level*/
    written_len = _yx_core_logger_package_level(&log_context, pcursor, buf_len);
    if (written_len > 0)
    {
        pcursor += written_len;
        buf_len -= written_len;
    }
    
    /*process_id*/
    written_len = _yx_core_logger_package_pid(&log_context, pcursor, buf_len);
    if (written_len > 0)
    {
        pcursor += written_len;
        buf_len -= written_len;
    }
    
    /*file*/
    written_len = _yx_core_logger_package_file(&log_context, pcursor, buf_len);
    if (written_len > 0)
    {
        pcursor += written_len;
        buf_len -= written_len;
    }
    
    /*func*/
    written_len = _yx_core_logger_package_func(&log_context, pcursor, buf_len);
    if (written_len > 0)
    {
        pcursor += written_len;
        buf_len -= written_len;
    }
    
    written_len = _yx_core_logger_package_line(&log_context, pcursor, buf_len);
    if (written_len > 0)
    {
        pcursor += written_len;
        buf_len -= written_len;
    }
    
    /*msg*/
    written_len = _yx_core_logger_package_msg(&log_context, pcursor, buf_len);
    if (written_len > 0)
    {
        pcursor += written_len;
        buf_len -= written_len;
    }
    
   
    /*new line*/
    written_len = _yx_core_logger_package_newline(&log_context, pcursor, buf_len);
    if (written_len > 0)
    {
        pcursor += written_len;
        buf_len -= written_len;
    }

    
    yx_vaend(log_context.valist);

    
    buff[YX_CORE_LOG_MAXBUFF_LEN] = '\0';

    printf("%s", buff);
//    yx_os_file_read(yx_os_file_stdout, buff, YX_CORE_LOG_MAXBUFF_LEN);
}









static yx_size _yx_core_logger_package_msg(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen)
{
    yx_size writtenlen = (yx_size)yx_os_vsnprintf(buf, buflen, contextRef->format_string, contextRef->valist);
//    buf[buflen] = '\0';
    
    return writtenlen;
}



static yx_size _yx_core_logger_package_date(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen)
{
    //     setlocale(LC_TIME, "el_GR.utf8");
    
    return yx_os_date_string(buf, buflen, contextRef->logger->rule.timefmt, contextRef->date);
}

static yx_size _yx_core_logger_package_file(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen)
{
    yx_size cpsize = YX_MIN(contextRef->file_len, buflen);
    
    yx_os_strncpy(buf, contextRef->file, cpsize);
    
    return cpsize - 1; /*减去'\0'*/
}


static yx_size _yx_core_logger_package_func(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen)
{
    yx_size cpsize = YX_MIN(contextRef->func_len, buflen);
    
    yx_os_strncpy(buf, contextRef->func, cpsize);
    
    return cpsize - 1; /*减去'\0'*/

}



static yx_size _yx_core_logger_package_line(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen)
{
    
    yx_char *p, *pend;
    yx_size len;
    
    yx_char tmp[YX_UINT64_LEN];
    pend = &tmp[YX_UINT64_LEN - 1];
    p = pend;
    
    if (buflen <= 0)
        return 0;
    
    
    yx_long line = contextRef->line;
    
    do{
        *--p = "0123456789"[line % 10];
        line /= 10;
    }while(line != 0  &&  p != tmp);
    
    
    len = (yx_size)(pend - p);
    YX_MIN(len, buflen);
    
    yx_os_memcpy(buf, p, len);
    
    return len;
}

static yx_size _yx_core_logger_package_newline(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen)
{
    
    yx_size len = YX_MIN(buflen, contextRef->logger->rule.newline_len);
    
    const yx_char* newline_str  = contextRef->logger->rule.newline_flag;
    
    
//    yx_size i;
//    for (i=0; i < len; i++)
//    {
//        buf[i] = newline_str[i];
//    }
    
    yx_os_memcpy(buf, newline_str, len);

    
    
    return len;
}

static yx_size _yx_core_logger_package_level(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen)
{
    
    const yx_char* str = YX_LOGGER_LEVEL_UNKNOWN_STR;
    yx_size len = (yx_size)YX_LOGGER_LEVEL_UNKNOWN_STRLEN;

    
    yx_logger_level level = contextRef->level;
    if (level > YX_LOGGER_OFF  &&  level < YX_LOGGER_ALL)
    {
        /*使用>和<是因为记录日志时，没有All和Off的等级*/
        
        str = YX_CORE_LEVEL_STR_MAP[level];
        len = (yx_size)YX_CORE_LEVEL_STRLEN_MAP[level];
    }
    
    
    len = YX_MIN(buflen, len);
    
    yx_os_memcpy(buf, str, len);
    
//    yx_size i;
//    for (i=0; i < len; i++)
//    {
//        buf[i] = str[i];
//    }
    
    return len;
}

static yx_size _yx_core_logger_package_pid(yx_core_log_context_ref contextRef, yx_char* buf, yx_size buflen)
{
    
    yx_unused(contextRef);
    
    
    yx_char *p, *pend;
    yx_size len;
    
    yx_char tmp[YX_UINT64_LEN];
    pend = &tmp[YX_UINT64_LEN - 1];
    p = pend;
    
    if (buflen <= 0)
        return 0;
    
    
    yx_long pid = yx_os_processinfo_get_current_pid();
    
    do{
        *--p = "0123456789"[pid % 10];
        pid /= 10;
    }while(pid != 0  &&  p != tmp);
    
    
    len = (yx_size)(pend - p);
    YX_MIN(len, buflen);
    
    yx_os_memcpy(buf, p, len);
    
    return len;

}






//
//  logMain.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/6.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_headers.h"
//#include "yx_core_logger.h"
#include "yx_core_logger_utility.h"

#include <locale.h>

//extern yx_char* yx_os_strrev(yx_char* buf, yx_size buflen, yx_char* src, yx_size srclen);

extern yx_size getTimerString(yx_os_date* date, yx_char* buff, yx_size buf_len, const yx_char* fmt);

int logmain(int argc, const char * argv[])
{
//    yx_core_logger logger;
//    yx_core_logger_init(yx_stdC_allocator(), &logger, "YXLib", YX_LOGGER_ALL, NULL, 0);
//    
//    YX_DEBUGLOG(&logger, "helloworld");
//    fflush(stdout);
    
//    char buf[1024];
//    printf("%s\n", yx_os_strrev(buf, 1024, "abcdefg", 7));
//    fflush(stdout);
//    
//    printf("%s\n", yx_os_strrev(buf, 1024, buf, strlen(buf)));
//    fflush(stdout);
//    
//    
//    
//    printf("%s\n", yx_os_strrev(buf, 6, "abcdefg", 7));
//    fflush(stdout);
//    
//    strncpy(buf, "abcdef", 1024);
//    printf("%s\n", yx_os_strrev(buf, 6, buf, strlen(buf)));
//    fflush(stdout);


    
#pragma unused(argc, argv)
//    char buff[1023+1];
//    
//    yx_os_date* date = yx_os_date_now(yx_false);
//    
//    getTimerString(date, buff, 1023, "[%Y-%m-%d %H:%M:%S]");
//    buff[1023] = '\0';
//    
//    printf("%s", buff);
    
    return 0;
}


yx_size getTimerString(yx_os_date* date, yx_char* buff, yx_size buf_len, const yx_char* fmt)
{
    YX_ASSERT(NULL != time);
    YX_ASSERT(NULL != buff);
    YX_ASSERT(NULL != fmt);
    
//     setlocale(LC_TIME, "el_GR.utf8");
    return strftime(buff, buf_len, fmt, date);
}

//yx_char* yx_os_strrev(yx_char* buf, yx_size buflen, yx_char* src, yx_size srclen)
//{
//    yx_size src_start, src_end;
//    yx_size dst_start, dst_end;
//    yx_int size = YX_MIN(buflen, srclen);
//    
//    assert( buf != NULL );
//    assert( src != NULL );
//    
//    dst_end = buflen - 1;
//    
//    if (buflen < srclen)
//    {
//        dst_start = 0;
//        dst_end = buflen - 1;
//        
//        src_start = srclen - buflen;
//        src_end = srclen - 1;
//    }
//    else if(buflen > srclen)
//    {
//        dst_start = 0;
//        dst_end = srclen - 1;
//        
//        src_start = 0;
//        src_end = srclen - 1;
//    }
//    else
//    {
//        src_start = 0;
//        src_end = srclen - 1;
//        
//        dst_start = 0;
//        dst_end = buflen - 1;
//    }
//    
//    
//    yx_char tmp;
//    
//    while( (src_start <= src_end)  &&  (dst_start <= dst_end) )
//    {
//        tmp = src[src_start];
//        buf[dst_start] = src[src_end];
//        buf[dst_end] = tmp;
//        
//        
//        src_start++;
//        src_end--;
//        
//        dst_start++;
//        dst_end--;
//    }
//    
//    buf[size] = '\0';
//    
//    return buf;
//}



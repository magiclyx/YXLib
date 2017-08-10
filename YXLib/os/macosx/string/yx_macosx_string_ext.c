//
//  yx_macosx_string_ext.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/8.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_string_ext.h"

yx_size yx_os_strrev(yx_char* buf, yx_size buflen, yx_char* str, yx_size strlen)
{
    yx_size str_start, str_end;
    yx_size dst_start, dst_end;
    yx_size len = YX_MIN(buflen-1, strlen); //减一是为了最后添加'\0'
    
    assert( buf != NULL );
    assert( str != NULL );
    
    dst_end = buflen - 1;
    
    dst_start = 0;
    if (buflen < strlen)
    {
        dst_end = buflen - 1;
        
        str_start = strlen - buflen;
        str_end = strlen - 1;
    }
    else if(buflen > strlen)
    {
        dst_end = strlen - 1;
        
        str_start = 0;
        str_end = strlen - 1;
    }
    else
    {
        dst_end = buflen - 1;
        
        str_start = 0;
        str_end = strlen - 1;
    }
    
    
    yx_char tmp;
    
    while( dst_start < len ) //因为dst_start总是从0开始，所以直接比较拷贝了多少字符串
    {
        tmp = str[str_start];
        buf[dst_start] = str[str_end];
        buf[dst_end] = tmp;
        
        str_start++;
        str_end--;
        
        dst_start++;
        dst_end--;
    }
    
    buf[len] = '\0'; //因为之前求长度的时候，对buflen-1， 所以这里可以直接取len为'\0'
    
    return len;
}

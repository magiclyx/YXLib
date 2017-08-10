//
//  yx_macosx_char.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/5.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_char.h"

yx_char* yx_os_char2str(yx_char c)
{
    static yx_char buf[2];
    buf[0] = c;
    buf[1] = '\0';
    
    return buf;
}


void yx_os_characterset_init(yx_os_characterset_ref set, const yx_char* string, register yx_strSize len)
{
    register const yx_char* s = string;
    
    while (len--  && *s != '\0')
    {
        set->map[*s >> 3] |= (1 << (*s & 7));
        s++;
    }
}


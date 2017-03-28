//
//  yx_macosx_char.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/5.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_CHAR_H_
#define _YX_MACOSX_CHAR_H_


#include "../basictypes/yx_macosx_types.h"

/*
 CR & LF
 */

#define YX_CR '\r'
#define YX_LF '\n'
#define YX_NEWLINE "\n"

/*
 basic operation
 */

#define yx_os_tolower(c)      (yx_char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define yx_os_toupper(c)      (yx_char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)

#define yx_os_isupper(c)      (c >= 'A'  &&  c <= 'Z')
#define yx_os_islower(c)      (c >= 'a'  &&  c <= 'z')

#define yx_os_isalpha(c)      ( yx_os_islower(c)  ||  yx_os_isupper(c) )

#define yx_os_isspace(c)      (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f')

#define yx_os_isdigit(c)      (c >= '0'  &&  c <= '9')


yx_char* yx_os_char2str(yx_char c);
#define yx_os_uchar2str(c) yx_os_char2str((yx_char)c)

/*
 character set
 */

typedef struct yx_os_characterset
{
    yx_uchar map[32];
}yx_os_characterset;

typedef struct yx_os_characterset* yx_os_characterset_ref;


void yx_os_characterset_init(yx_os_characterset_ref set, const yx_char* string, yx_strSize len);



#endif /* defined(_YX_MACOSX_CHAR_H_) */

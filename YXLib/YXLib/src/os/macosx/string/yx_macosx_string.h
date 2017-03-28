//
//  yx_macosx_string.h
//  YXLib
//
//  Created by Yuxi Liu on 9/30/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_STRING_H_
#define _YX_MACOSX_STRING_H_

#include "../basictypes/yx_macosx_types.h"
#include "yx_macosx_char.h"



/*
 convert
 */
yx_char* yx_os_strlow(yx_char *dst, yx_char *src, yx_strSize len);
yx_char* yx_os_strup(yx_char *dst, yx_char *src, yx_strSize len);
yx_char* yx_os_strfillstr(yx_char* dst, yx_char* src, yx_strSize len);


/*
 property
 */
#define yx_os_strlen(str)               strlen((const char *) str)


/*
 compare
 */

#define yx_os_strncmp(str1, str2, len)  strncmp((const char *) str1, (const char *) str2, len)
yx_int yx_os_strncasecmp(yx_char* str1, yx_char* str2, yx_strSize len); //for 7-bit ASCII strings only
yx_int yx_os_strcasecmp(yx_char* str1, yx_char* str2); //for 7-bit ASCII strings only

yx_int yx_os_rstrncmp(yx_char *str1, yx_char *str2, yx_strSize len);
yx_int yx_os_rstrncasecmp(yx_char *str1, yx_char *str2, yx_strSize len);

yx_int yx_os_filename_cmp(yx_char *path1, yx_char *path2, yx_strSize len);



/*
 search
 */

#define yx_os_strstr(str, search)       strstr((const char *) str, (const char *) searching)
yx_char* yx_os_strstrn(yx_char* string, yx_char* search, yx_strSize len); //string must a null-terminal string
yx_char* yx_os_strnstr(yx_char* string, yx_char* search, yx_strSize len); //search must a null-terminal string
yx_char *yx_os_strcasestrn(yx_char *string, char *search, yx_strSize len);  //string must a null-terminal string
yx_char *yx_os_strlcasestrn(yx_char *string, yx_char *string_last, yx_char *search, yx_strSize len); //len is the length of the search string


#define yx_os_strchr(str, c)            strchr((const char *) str, (int) c)
yx_char* yx_os_strlchr(yx_char *p, yx_char *last, yx_char c);



/*
 number
 */
yx_int yx_os_atoi(const yx_char* str); //失败返回0
yx_int yx_os_atoin(const yx_char* str, yx_strSize len); //失败返回0

yx_long yx_os_atol_base(const yx_char* str, yx_char** endPtr, yx_int base); //base 0, 2~36. endPtr输出匹配失败的字符
yx_ulong yx_os_atoul_base(const yx_char *str, yx_char **endPtr, yx_int base);

//fast 函数，不能处理空格和负数，在len范围内必须全部是数字，失败返回-1
yx_int yx_os_atoin_fast(const yx_char* str, yx_strSize len);
yx_int yx_os_atofp_fast(const yx_char* str, yx_strSize n, yx_strSize point);//parse a fixed point number, e.g., ngx_atofp("10.5", 4, 2) returns 1050
yx_int yx_os_hextoi_fast(const yx_char* str, yx_strSize len);


//数字转字符串

yx_char* yx_ltoa(yx_long i);
#define yx_itoa(val) yx_ltoa((yx_int)val)

yx_char* yx_ftoa_ex(yx_real f, yx_char* buf, yx_int precision);
yx_char* yx_ftoa(yx_real f);

//普通的将数字转换为字符串
yx_char* yx_os_num2str(yx_char* dst, yx_strSize len, yx_int num, yx_int base);
//把数字的字符串转换为16进制数字的字符串(速度快)
yx_char* yx_os_hex2str(yx_char* dst, const yx_char* src, yx_strSize len);



/*
 io
 */
#define yx_os_printf  printf
#define yx_os_snprintf(buff, buff_len, fmt, args...)     snprintf(buff, buff_len, fmt, args)

#define yx_os_vsnprintf(buff, buff_len, format, valist)  vsnprintf(buff, buff_len, format, valist)
#define yx_os_vprintf(fmt, valist)    vprintf(fmt, valist)
#define yx_os_vfprintf(file, fmt, valist)    vfprintf(file, fmt, valist)



/*
 copy & cat
 */
#define yx_os_strncpy(dst, src, len)  strncpy((char*)dst, (const char*)src, len)
#define yx_os_strncat(str1, str2, str1_len) strncat(str1, str2, str1_len)



/*
 clear & empty
 */
#define yx_os_strempty(str) (str[0]='\0')




#endif

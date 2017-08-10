//
//  yx_macosx_string_http.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/5.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_STRING_HTTP_H_
#define _YX_MACOSX_STRING_HTTP_H_ 

#include "yx_macosx_string_base64.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/*url base64*/
////////////////////////////////////////////////////////////////////////////////////////////////////
void yx_os_encode_base64url(yx_char* dst, const yx_char* src, yx_size* dstSize, yx_size srcSize);
yx_int yx_os_decode_base64url(yx_char* dst, const yx_char* src, yx_size* dstSize, yx_size srcSize);

////////////////////////////////////////////////////////////////////////////////////////////////////
/*escape uri*/
////////////////////////////////////////////////////////////////////////////////////////////////////

#define YX_OS_ESCAPE_URI            0
#define YX_OS_ESCAPE_ARGS           1
#define YX_OS_ESCAPE_URI_COMPONENT  2
#define YX_OS_ESCAPE_HTML           3
#define YX_OS_ESCAPE_REFRESH        4
#define YX_OS_ESCAPE_MEMCACHED      5
#define YX_OS_ESCAPE_MAIL_AUTH      6

#define YX_OS_UNESCAPE_URI       1
#define YX_OS_UNESCAPE_REDIRECT  2

/*
 对src进行编码，根据type来按不同的方式进行编码，如果dst为NULL，则返回需要转义的字符的数量，由此可得到需要的空间大小
 
 如果dit为空，返回需要的空间大小
 如果不为空，返回dst
 */
yx_value yx_os_escape_uri(yx_char *dst, yx_char *src, yx_size size, yx_uint type);

/*
对src进行反编码
type可以是0、YX_OS_UNESCAPE_URI、YX_OS_UNESCAPE_REDIRECT这三个值。
 如果是0，则表示src中的所有字符都要进行转码。如果是YX_OS_UNESCAPE_URI与YX_OS_UNESCAPE_REDIRECT，则遇到’?’后就结束了，后面的字符就不管了。
 而YX_OS_UNESCAPE_URI与YX_OS_UNESCAPE_REDIRECT之间的区别是YX_OS_UNESCAPE_URI对于遇到的需要转码的字符，都会转码，而YX_OS_UNESCAPE_REDIRECT则只会对非可见字符进行转码。
 */
void yx_os_unescape_uri(yx_char **dst, yx_char **src, yx_size size, yx_uint type);



/*escape htmp*/
yx_value yx_os_escape_html(yx_char *dst, yx_char *src, yx_size size);


#endif




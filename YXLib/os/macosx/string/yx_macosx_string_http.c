//
//  yx_macosx_string_http.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/5.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_string_http.h"

void yx_os_encode_base64url(yx_char* dst, const yx_char* src, yx_size* dstSize, yx_size srcSize)
{
    static yx_uchar  basis64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    
    yx_os_encode_base64_internal(dst, src, dstSize, srcSize, basis64, 0);
    
}

yx_int yx_os_decode_base64url(yx_char* dst, const yx_char* src, yx_size* dstSize, yx_size srcSize)
{
    static yx_uchar   basis64[] = {
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 62, 77, 77,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 77, 77, 77, 77, 77,
        77,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 77, 77, 77, 77, 63,
        77, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 77, 77, 77, 77, 77,
        
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77
    };
    
    return yx_os_decode_base64_internal(dst, src, dstSize, srcSize, basis64);
}



yx_value yx_os_escape_uri(yx_char *dst, yx_char *src, yx_size size, yx_uint type)
{
    yx_value n;
    yx_uint32 *escape;
    static yx_uchar hex[] = "0123456789ABCDEF";
    
    /* " ", "#", "%", "?", %00-%1F, %7F-%FF */
    
    static yx_uint32   uri[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        
        /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x80000029, /* 1000 0000 0000 0000  0000 0000 0010 1001 */
        
        /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
        
        /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */
        
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };
    
    /* " ", "#", "%", "&", "+", "?", %00-%1F, %7F-%FF */
    
    static yx_uint32   args[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        
        /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x88000869, /* 1000 1000 0000 0000  0000 1000 0110 1001 */
        
        /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
        
        /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */
        
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };
    
    /* not ALPHA, DIGIT, "-", ".", "_", "~" */
    
    static yx_uint32   uri_component[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        
        /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0xfc009fff, /* 1111 1100 0000 0000  1001 1111 1111 1111 */
        
        /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x78000001, /* 0111 1000 0000 0000  0000 0000 0000 0001 */
        
        /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0xb8000001, /* 1011 1000 0000 0000  0000 0000 0000 0001 */
        
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };
    
    /* " ", "#", """, "%", "'", %00-%1F, %7F-%FF */
    
    static yx_uint32   html[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        
        /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x000000ad, /* 0000 0000 0000 0000  0000 0000 1010 1101 */
        
        /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
        
        /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */
        
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };
    
    /* " ", """, "%", "'", %00-%1F, %7F-%FF */
    
    static yx_uint32   refresh[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        
        /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x00000085, /* 0000 0000 0000 0000  0000 0000 1000 0101 */
        
        /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
        
        /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */
        
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };
    
    /* " ", "%", %00-%1F */
    
    static yx_uint32   memcached[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        
        /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x00000021, /* 0000 0000 0000 0000  0000 0000 0010 0001 */
        
        /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
        
        /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
        
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
    };
    
    /* mail_auth is the same as memcached */
    
    static yx_uint32  *map[] =
    { uri, args, uri_component, html, refresh, memcached, memcached };
    
    
    escape = map[type];
    
    if (dst == NULL) {
        
        /* find the number of the characters to be escaped */
        
        n = 0;
        
        while (size) {
            if (escape[*src >> 5] & (1 << (*src & 0x1f))) {
                n++;
            }
            src++;
            size--;
        }
        
        return (yx_value)n;
    }
    
    while (size) {
        if (escape[*src >> 5] & (1 << (*src & 0x1f))) {
            *dst++ = '%';
            *dst++ = (yx_char)(hex[*src >> 4]);
            *dst++ = (yx_char)(hex[*src & 0xf]);
            src++;
            
        } else {
            *dst++ = *src++;
        }
        size--;
    }
    
    return (yx_value)dst;

}


void yx_os_unescape_uri(yx_char **dst, yx_char **src, yx_size size, yx_uint type)
{
    yx_uchar *d, *s, ch, c, decoded;
    enum {
        sw_usual = 0,
        sw_quoted,
        sw_quoted_second
    } state;
    
    d = (yx_uchar*)(*dst);
    s = (yx_uchar*)(*src);
    
    state = 0;
    decoded = 0;
    
    while (size--) {
        
        ch = *s++;
        
        switch (state) {
            case sw_usual:
                if (ch == '?'
                    && (type & (YX_OS_UNESCAPE_URI|YX_OS_UNESCAPE_REDIRECT)))
                {
                    *d++ = ch;
                    goto done;
                }
                
                if (ch == '%') {
                    state = sw_quoted;
                    break;
                }
                
                *d++ = ch;
                break;
                
            case sw_quoted:
                
                if (ch >= '0' && ch <= '9') {
                    decoded = (u_char) (ch - '0');
                    state = sw_quoted_second;
                    break;
                }
                
                c = (u_char) (ch | 0x20);
                if (c >= 'a' && c <= 'f') {
                    decoded = (u_char) (c - 'a' + 10);
                    state = sw_quoted_second;
                    break;
                }
                
                /* the invalid quoted character */
                
                state = sw_usual;
                
                *d++ = ch;
                
                break;
                
            case sw_quoted_second:
                
                state = sw_usual;
                
                if (ch >= '0' && ch <= '9') {
                    ch = (u_char) ((decoded << 4) + ch - '0');
                    
                    if (type & YX_OS_UNESCAPE_REDIRECT) {
                        if (ch > '%' && ch < 0x7f) {
                            *d++ = ch;
                            break;
                        }
                        
                        *d++ = '%'; *d++ = *(s - 2); *d++ = *(s - 1);
                        
                        break;
                    }
                    
                    *d++ = ch;
                    
                    break;
                }
                
                c = (u_char) (ch | 0x20);
                if (c >= 'a' && c <= 'f') {
                    ch = (u_char) ((decoded << 4) + c - 'a' + 10);
                    
                    if (type & YX_OS_UNESCAPE_URI) {
                        if (ch == '?') {
                            *d++ = ch;
                            goto done;
                        }
                        
                        *d++ = ch;
                        break;
                    }
                    
                    if (type & YX_OS_UNESCAPE_REDIRECT) {
                        if (ch == '?') {
                            *d++ = ch;
                            goto done;
                        }
                        
                        if (ch > '%' && ch < 0x7f) {
                            *d++ = ch;
                            break;
                        }
                        
                        *d++ = '%'; *d++ = *(s - 2); *d++ = *(s - 1);
                        break;
                    }
                    
                    *d++ = ch;
                    
                    break;
                }
                
                /* the invalid quoted character */
                
                break;
        }
    }
    
done:
    
    *dst = (yx_char*)d;
    *src = (yx_char*)s;

}




yx_value yx_os_escape_html(yx_char *dst, yx_char *src, yx_size size)
{
    yx_uchar ch;
    yx_uint len;
    
    if (dst == NULL) {
        
        len = 0;
        
        while (size) {
            switch (*src++) {
                    
                case '<':
                    len += sizeof("&lt;") - 2;
                    break;
                    
                case '>':
                    len += sizeof("&gt;") - 2;
                    break;
                    
                case '&':
                    len += sizeof("&amp;") - 2;
                    break;
                    
                case '"':
                    len += sizeof("&quot;") - 2;
                    break;
                    
                default:
                    break;
            }
            size--;
        }
        
        return (yx_value) len;
    }
    
    while (size) {
        ch = (yx_uchar)(*src++);
        
        switch (ch) {
                
            case '<':
                *dst++ = '&'; *dst++ = 'l'; *dst++ = 't'; *dst++ = ';';
                break;
                
            case '>':
                *dst++ = '&'; *dst++ = 'g'; *dst++ = 't'; *dst++ = ';';
                break;
                
            case '&':
                *dst++ = '&'; *dst++ = 'a'; *dst++ = 'm'; *dst++ = 'p';
                *dst++ = ';';
                break;
                
            case '"':
                *dst++ = '&'; *dst++ = 'q'; *dst++ = 'u'; *dst++ = 'o';
                *dst++ = 't'; *dst++ = ';';
                break;
                
            default:
                *dst++ = (yx_char)ch;
                break;
        }
        size--;
    }
    
    return (yx_value) dst;

}



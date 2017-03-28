//
//  yx_macosx_string_encode.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/5.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_string_base64.h"

void
yx_os_encode_base64_internal(yx_char* dst, const yx_char* src, yx_size* dstSize, yx_size srcSize, const yx_uchar* basis, yx_uint padding)
{
    register yx_uchar *d, *s;
    register yx_size len;
    
    len = srcSize;
    s = (yx_uchar*)src;
    d = (yx_uchar*)dst;
    
    while (len > 2) {
        *d++ = basis[(s[0] >> 2) & 0x3f];
        *d++ = basis[((s[0] & 3) << 4) | (s[1] >> 4)];
        *d++ = basis[((s[1] & 0x0f) << 2) | (s[2] >> 6)];
        *d++ = basis[s[2] & 0x3f];
        
        s += 3;
        len -= 3;
    }
    
    if (len) {
        *d++ = basis[(s[0] >> 2) & 0x3f];
        
        if (len == 1) {
            *d++ = basis[(s[0] & 3) << 4];
            if (padding) {
                *d++ = '=';
            }
            
        } else {
            *d++ = basis[((s[0] & 3) << 4) | (s[1] >> 4)];
            *d++ = basis[(s[1] & 0x0f) << 2];
        }
        
        if (padding) {
            *d++ = '=';
        }
    }
    
    *dstSize = (yx_size)(d - (yx_uchar*)dst);
}



yx_int
yx_os_decode_base64_internal(yx_char* dst, const yx_char* src, yx_size* dstSize, yx_size srcSize, const yx_uchar *basis)
{
    register yx_size len, src_len;
    register yx_uchar *d, *s;
    
    src_len = srcSize;
    
    for (len = 0; len < src_len; len++) {
        if (src[len] == '=') {
            break;
        }
        
        if (basis[src[len]] == 77) {
            return -1;
        }
    }
    
    if (len % 4 == 1) {
        return -1;
    }
    
    s = (yx_uchar*)src;
    d = (yx_uchar*)dst;
    
    while (len > 3) {
        *d++ = (yx_uchar) (basis[s[0]] << 2 | basis[s[1]] >> 4);
        *d++ = (yx_uchar) (basis[s[1]] << 4 | basis[s[2]] >> 2);
        *d++ = (yx_uchar) (basis[s[2]] << 6 | basis[s[3]]);
        
        s += 4;
        len -= 4;
    }
    
    if (len > 1) {
        *d++ = (yx_uchar) (basis[s[0]] << 2 | basis[s[1]] >> 4);
    }
    
    if (len > 2) {
        *d++ = (yx_uchar) (basis[s[1]] << 4 | basis[s[2]] >> 2);
    }
    
    *dstSize = (yx_size)(d - (yx_uchar)dst);
    
    return 0;
}



void yx_os_encode_base64(yx_char* dst, const yx_char* src, yx_size* dstSize, yx_size srcSize)
{
    static yx_uchar   basis64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    yx_os_encode_base64_internal(dst, src, dstSize, srcSize, basis64, 1);
}



yx_int yx_os_decode_base64(yx_char* dst, const yx_char* src, yx_size* dstSize, yx_size srcSize)
{
    static yx_uchar  basis64[] = {
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 62, 77, 77, 77, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 77, 77, 77, 77, 77,
        77,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 77, 77, 77, 77, 77,
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




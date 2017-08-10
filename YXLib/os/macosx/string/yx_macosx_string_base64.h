//
//  yx_macosx_string_encode.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/5.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_STRING_BASE64_H_
#define _YX_MACOSX_STRING_BASE64_H_

#include "../basictypes/yx_macosx_types.h"

/*base 64*/
#define yx_os_base64_encoded_length(len)  (((len + 2) / 3) * 4)
#define yx_os_base64_decoded_length(len)  (((len + 3) / 4) * 3)

void yx_os_encode_base64_internal(yx_char* dst, const yx_char* src, yx_size* dstSize, yx_size srcSize, const yx_uchar* basis, yx_uint padding);

yx_int yx_os_decode_base64_internal(yx_char* dst, const yx_char* src, yx_size* dstSize, yx_size srcSize, const yx_uchar *basis);



void yx_os_encode_base64(yx_char* dst, const yx_char* src, yx_size* dstSize, yx_size srcSize);

yx_int yx_os_decode_base64(yx_char* dst, const yx_char* src, yx_size* dstSize, yx_size srcSize);
yx_int yx_os_decode_base64url(yx_char* dst, const yx_char* src, yx_size* dstSize, yx_size srcSize);




#endif

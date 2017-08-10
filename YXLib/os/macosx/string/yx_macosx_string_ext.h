//
//  yx_macosx_string_ext.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/8.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_STRING_EXT_H_
#define _YX_MACOSX_STRING_EXT_H_

#include "../basictypes/yx_macosx_types.h"

/*
 字符串反转
 buf 和 str可以是同一块内存，也可以是不同的内存。但是两段内存不能是重叠的，否则会结果会错误
 实际上， buflen指的是buf的大小， strlen指的是字符串长度。
 因为有'\0'的存在，所以buflen应该笔strlen大至少1个长度
 */
yx_size yx_os_strrev(yx_char* buf, yx_size buflen, yx_char* str, yx_size strlen);

#endif /* _YX_MACOSX_STRING_EXT_H_ */

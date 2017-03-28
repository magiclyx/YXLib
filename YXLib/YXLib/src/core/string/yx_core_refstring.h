//
//  yx_core_refstring.h
//  YXLib
//
//  Created by LiuYuxi on 15/8/14.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_REFSTRING_H_
#define _YX_CORE_REFSTRING_H_

#include "../yx_core_global.h"

typedef struct yx_core_stringnode{
    yx_char* string;
    yx_size len;
}yx_core_stringnode;

typedef struct yx_core_string_node* yx_core_stringnode_ref;


#define yx_core_str(cstring) {cstring, sizeof(cstring) - 1}

#define yx_core_str_null {NULL, 0}

#define yx_core_str_init(strnode, cstring) \
(strnode)->string = (yx_char*) cstring; (strnode)->len = sizeof(cstring) - 1

#define yx_core_str_init_null(strnode) \
(strnode)->string = NULL; (strnode)->len = 0

#define yx_core_str_clear(strnode) \
strnode->string = NULL; strnode->len = 0



#endif /* defined(_YX_CORE_REFSTRING_H_) */

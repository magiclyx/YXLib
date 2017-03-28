//
//  yx_core_string_buf.h
//  YXLib
//
//  Created by yuxi on 16/8/1.
//  Copyright © 2016年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_STRING_BUF_H_
#define _YX_CORE_STRING_BUF_H_


#include "../yx_core_global.h"
#include "../container/yx_core_listnode.h"

typedef struct yx_core_string_buf{
    yx_char* pstr;
    yx_size len;
    yx_bool validate;
    yx_allocator allocator;
}yx_core_string;



#endif /* _YX_CORE_STRING_BUF_H_ */

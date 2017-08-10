//
//  yx_core_string.h
//  YXLib
//
//  Created by yuxi on 16/8/1.
//  Copyright © 2016年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_STRING_H_
#define _YX_CORE_STRING_H_


#include "../yx_core_global.h"
#include "../buff/yx_core_invbuf.h"

typedef struct yx_core_string{
    yx_core_infbuf buff;
    yx_size len;
}yx_core_string, *yx_core_string_ref;


//- (void)yx_core_string_init_cstring(yx_core_string_ref string);
//- (void)yx_core_string_init_cstring_nocopy(yx_core_string_ref string);
//
//- (void)yx_core_string_init_string(yx_core_string_ref string, yx_core_string_ref srcString);
//
//- (void)yx_core_string_recycle(yx_core_string_ref* string_ptr);






#endif /* _YX_CORE_STRING_H_ */

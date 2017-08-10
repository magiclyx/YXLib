//
//  yx_core_autoString.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/9.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_AUTOSTRING_H_
#define _YX_CORE_AUTOSTRING_H_

#include "../yx_core_global.h"
#include "yx_core_auto_interface.h"

typedef struct yx_core_autoString{
    YX_AUTOTYPE_STRUCT_DECLARE;
    
    yx_allocator allocator;
    
    yx_size len;
    yx_char* data;
    
    yx_uint nullterminate : 1;  /*是否为null-terminal字符串*/
    yx_uint referenced : 1; /*是否是引用字符串*/
}yx_core_autoString;




#endif /* _YX_CORE_AUTOSTRING_H_ */

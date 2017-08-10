//
//  yx_core_object.c
//  YXLib
//
//  Created by yuxi on 16/8/3.
//  Copyright © 2016年 Yuxi Liu. All rights reserved.
//

#include "yx_core_object.h"

const yx_char* yx_core_object_description(yx_core_object_ref object, yx_char* buff, yx_strSize buff_len)
{
    yx_unused(buff);
    yx_unused(buff_len);
    
    switch (object->type) {
            
        case yx_core_type_char:
            return yx_os_char2str(object->value.charValue);
        case yx_core_type_uchar:
            return yx_os_uchar2str(object->value.ucharValue);
        case yx_core_type_int:
            return yx_itoa(object->value.intValue);
        case yx_core_type_uint:
            return yx_itoa((yx_int)object->value.intValue);
        case yx_core_type_long:
            return yx_ltoa(object->value.longValue);
        case yx_core_type_ulong:
            return yx_ltoa((yx_long)(object->value.longValue));
        case yx_core_type_longlong:
            return yx_ltoa(object->value.longValue);
        case yx_core_type_ulonglong:
            return yx_ltoa((yx_long)(object->value.longValue));
        case yx_core_type_real:
            return yx_ftoa(object->value.realValue);
        case yx_core_type_ptr:
        {
//            static yx_char buf[17];
//            return yx_os_num2str(buf, 17, (yx_ulong)(object->value.ptrValue), 16);
        }
        case yx_core_type_list:
            break;
        case yx_core_type_rbtree:
            break;
        case yx_core_type_queue:
            break;
        case yx_core_type_hashTable:
            break;
            
        default:
            break;
    }
    
    return NULL;
}


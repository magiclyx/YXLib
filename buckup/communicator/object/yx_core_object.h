//
//  yx_core_object.h
//  YXLib
//
//  Created by yuxi on 16/8/1.
//  Copyright © 2016年 Yuxi Liu. All rights reserved.
//

#ifndef yx_core_object_h
#define yx_core_object_h

#include "../yx_core_global.h"
#include "../container/yx_core_list.h"
#include "../container/yx_core_queue.h"
#include "../container/yx_core_rbtree.h"
#include "../container/yx_core_hashtable.h"


typedef enum {
    yx_core_type_char,
    yx_core_type_uchar,
    
    yx_core_type_int,
    yx_core_type_uint,
    
    yx_core_type_long,
    yx_core_type_ulong,
    
    yx_core_type_longlong,
    yx_core_type_ulonglong,
    
    yx_core_type_real,
    
    yx_core_type_ptr,
    
    yx_core_type_list,
    yx_core_type_rbtree,
    yx_core_type_queue,
    yx_core_type_hashTable
}yx_core_object_type;


typedef union {
    yx_char charValue;
    yx_uchar ucharValue;
    
    yx_int intValue;
    yx_uint uintValue;
    
    yx_long longValue;
    yx_ulong ulongValue;
    
    yx_longlong longlongValue;
    yx_ulonglong ulonglongValue;
    
    yx_real realValue;
    
    yx_ptr ptrValue;
    
    yx_core_list_ref listValue;
    yx_core_rbtree_ref rbtreeValue;
    yx_core_queue_ptr queueValue;
    yx_core_hashtable_ref hashTableValue;
    
}yx_core_val;



typedef struct yx_core_object{
    yx_core_val value;
    yx_core_object_type type;
}yx_core_object, *yx_core_object_ref;



const yx_char* yx_core_object_description(yx_core_object_ref object, yx_char* buff, yx_strSize buff_len);


#endif /* yx_core_object_h */



















































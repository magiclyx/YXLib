//
//  yx_core_threadsafe_hashtable.h
//  YXCLib
//
//  Created by Yuxi Liu on 2017/11/27.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_THREADSAFE_HASHTABLE_H_
#define _YX_CORE_THREADSAFE_HASHTABLE_H_

#include "../../yx_core_global.h"
#include "../yx_core_hashtable.h"

typedef struct yx_core_tshashtable{
    yx_core_hashtable hashtable;
    yx_os_pthread_rwlock_type_declare(rwlock);
}yx_core_threadsafe_hashtable;

typedef struct yx_core_tshashtable* yx_core_tshashtable_ref;


/*
 create & destroy
 */
yx_core_tshashtable_ref yx_core_tshashtable_create(yx_allocator allocator, yx_size size);
void yx_core_tshashtable_init(yx_allocator allocator, yx_core_tshashtable_ref tshashtable, yx_size size);

void yx_core_tshashtable_recycle(yx_core_tshashtable_ref tshashtable);
void yx_core_tshashtable_destroy(yx_core_tshashtable_ref* tshashtable_ptr);


/*
 set callback function
 */

#define yx_core_tshashtable_set_callback_compare(tshashtable_ref, compare_fun) \
    yx_core_hashtable_set_callback_compare(&((tshashtable_ref)->hashtable), compare_fun)

#define yx_core_tshashtable_set_callback_del(tshashtable_ref, del_fun) \
    yx_core_hashtable_set_callback_del(&((tshashtable_ref)->hashtable), del_fun)

#define yx_core_tshashtable_set_callback_keycopy(tshashtable_ref, copy_fun) \
    yx_core_hashtable_set_callback_keycopy(&((tshashtable_ref)->hashtable), copy_fun)

#define yx_core_tshashtable_set_callback_hash(tshashtable_ref, hash_fun) \
    yx_core_hashtable_set_callback_hash(&((tshashtable_ref)->hashtable), hash_fun)


/*
 property
 */
yx_size yx_core_tshashtable_count(yx_core_tshashtable_ref tshashtble_ref);
yx_bool yx_core_tshashtable_isEmpty(yx_core_tshashtable_ref tshashtble_ref);
#define yx_core_tshashtable_hashValue(tshashtable_ref, key) \
    yx_core_hashtable_hashValue(&((tshashtable_ref)->hashtable), key)


/*
 set value for key
 */
//return yx_true if a value exist with the same key, else return yx_false
yx_bool yx_core_tshashtable_set(yx_core_tshashtable_ref tshashtble_ref, yx_value key, yx_value value);
yx_bool yx_core_tshashtable_setWithHashKey(yx_core_tshashtable_ref tshashtble_ref, yx_value key, yx_core_hash hashkey, yx_value value);



/*
 get value for key
 */
yx_value yx_core_tshashtable_get(yx_core_tshashtable_ref tshashtble_ref, yx_value key, yx_value defaultValue);
yx_value yx_core_tshashtable_getWithHashkey(yx_core_tshashtable_ref hashtable, yx_value key, yx_core_hash hashkey, yx_value defaultValue);



/*
 remove
 */
yx_bool yx_core_tshashtable_remove(yx_core_tshashtable_ref hashtable, yx_value key);
yx_bool yx_core_tshashtable_removeWithHashkey(yx_core_tshashtable_ref hashtable, yx_value key, yx_core_hash hashkey);


/*
 test
 */
yx_bool yx_core_tshashtable_isExist(yx_core_tshashtable_ref hashtable, yx_value key);
yx_bool yx_core_tshashtable_isExistWithHashkey(yx_core_tshashtable_ref hashtable, yx_value key, yx_core_hash hashkey);



#endif /* yx_core_threadsafe_hashtable_h */











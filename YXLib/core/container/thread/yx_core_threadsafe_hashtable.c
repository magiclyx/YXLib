//
//  yx_core_threadsafe_hashtable.c
//  YXCLib
//
//  Created by Yuxi Liu on 2017/11/27.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#include "yx_core_threadsafe_hashtable.h"

/*
 create & destroy
 */
yx_core_tshashtable_ref yx_core_tshashtable_create(yx_allocator allocator, yx_size size)
{
    yx_core_tshashtable_ref tshashtble = (yx_core_tshashtable_ref)yx_allocator_alloc(allocator, sizeof(yx_core_threadsafe_hashtable));
    
    yx_core_tshashtable_init(allocator, tshashtble, size);
    
    return tshashtble;
}

void yx_core_tshashtable_init(yx_allocator allocator, yx_core_tshashtable_ref tshashtable, yx_size size)
{
    if (NULL == allocator)
        allocator = yx_get_allocator();
    
    yx_os_pthread_rwlock_init(&(tshashtable->rwlock));
    yx_core_hashtable_init(allocator, &(tshashtable->hashtable), size);
}

void yx_core_tshashtable_recycle(yx_core_tshashtable_ref tshashtable)
{
    yx_core_hashtable_recycle(&(tshashtable->hashtable));
    
    yx_os_pthread_rwlock_recycle(&(tshashtable->rwlock));
}

void yx_core_tshashtable_destroy(yx_core_tshashtable_ref* tshashtable_ptr)
{
    yx_core_tshashtable_ref hashtable = *(tshashtable_ptr);
    *(tshashtable_ptr) = NULL;
    
    yx_core_tshashtable_recycle(hashtable);
}


/*
 property
 */
yx_size yx_core_tshashtable_count(yx_core_tshashtable_ref tshashtble_ref)
{
    yx_size size = 0;
    
    yx_os_pthread_rwlock_rlock(&(tshashtble_ref->rwlock));
    size = yx_core_hashtable_count(&(tshashtble_ref->hashtable));
    yx_os_pthread_rwlock_unlock(&(tshashtble_ref->rwlock));
    
    return size;
}

yx_bool yx_core_tshashtable_isEmpty(yx_core_tshashtable_ref tshashtble_ref)
{
    yx_bool isEmpty = yx_true;
    
    yx_os_pthread_rwlock_rlock(&(tshashtble_ref->rwlock));
    isEmpty = yx_core_hashtable_isEmpty(&(tshashtble_ref->hashtable));
    yx_os_pthread_rwlock_unlock(&(tshashtble_ref->rwlock));
    
    return isEmpty;
}

/*
 set value for key
 */
//return yx_true if a value exist with the same key, else return yx_false
yx_bool yx_core_tshashtable_set(yx_core_tshashtable_ref tshashtble_ref, yx_value key, yx_value value)
{
    yx_bool rt;
    
    yx_os_pthread_rwlock_wlock(&(tshashtble_ref->rwlock));
    rt = yx_core_hashtable_set(&(tshashtble_ref->hashtable), key, value);
    yx_os_pthread_rwlock_unlock(&(tshashtble_ref->rwlock));

    return rt;
}

yx_bool yx_core_tshashtable_setWithHashKey(yx_core_tshashtable_ref tshashtble_ref, yx_value key, yx_core_hash hashkey, yx_value value)
{
    yx_bool rt;
    
    yx_os_pthread_rwlock_wlock(&(tshashtble_ref->rwlock));
    rt = yx_core_hashtable_setWithHashKey(&(tshashtble_ref->hashtable), key, hashkey, value);
    yx_os_pthread_rwlock_unlock(&(tshashtble_ref->rwlock));
    
    return rt;
}



/*
 get value for key
 */
yx_value yx_core_tshashtable_get(yx_core_tshashtable_ref tshashtble_ref, yx_value key, yx_value defaultValue)
{
    yx_value value;
    
    yx_os_pthread_rwlock_rlock(&(tshashtble_ref->rwlock));
    value = yx_core_hashtable_get(&(tshashtble_ref->hashtable), key, defaultValue);
    yx_os_pthread_rwlock_unlock(&(tshashtble_ref->rwlock));
    
    return value;
}

yx_value yx_core_tshashtable_getWithHashkey(yx_core_tshashtable_ref tshashtble_ref, yx_value key, yx_core_hash hashkey, yx_value defaultValue)
{
    yx_value value;
    
    yx_os_pthread_rwlock_rlock(&(tshashtble_ref->rwlock));
    value = yx_core_hashtable_getWithHashkey(&(tshashtble_ref->hashtable), key, hashkey, defaultValue);
    yx_os_pthread_rwlock_unlock(&(tshashtble_ref->rwlock));
    
    return value;
}



/*
 remove
 */
yx_bool yx_core_tshashtable_remove(yx_core_tshashtable_ref tshashtble_ref, yx_value key)
{
    yx_bool rt;
    
    yx_os_pthread_rwlock_wlock(&(tshashtble_ref->rwlock));
    rt = yx_core_hashtable_remove(&(tshashtble_ref->hashtable), key);
    yx_os_pthread_rwlock_unlock(&(tshashtble_ref->rwlock));

    return rt;
}

yx_bool yx_core_tshashtable_removeWithHashkey(yx_core_tshashtable_ref tshashtble_ref, yx_value key, yx_core_hash hashkey)
{
    yx_bool rt;
    
    yx_os_pthread_rwlock_wlock(&(tshashtble_ref->rwlock));
    rt = yx_core_hashtable_removeWithHashkey(&(tshashtble_ref->hashtable), key, hashkey);
    yx_os_pthread_rwlock_unlock(&(tshashtble_ref->rwlock));
    
    return rt;
}


/*
 test
 */
yx_bool yx_core_tshashtable_isExist(yx_core_tshashtable_ref tshashtble_ref, yx_value key)
{
    yx_bool rt;
    
    yx_os_pthread_rwlock_rlock(&(tshashtble_ref->rwlock));
    rt = yx_core_hashtable_isExist(&(tshashtble_ref->hashtable), key);
    yx_os_pthread_rwlock_unlock(&(tshashtble_ref->rwlock));
    
    return rt;
}
yx_bool yx_core_tshashtable_isExistWithHashkey(yx_core_tshashtable_ref tshashtble_ref, yx_value key, yx_core_hash hashkey)
{
    yx_bool rt;
    
    yx_os_pthread_rwlock_rlock(&(tshashtble_ref->rwlock));
    rt = yx_core_hashtable_isExistWithHashkey(&(tshashtble_ref->hashtable), key, hashkey);
    yx_os_pthread_rwlock_unlock(&(tshashtble_ref->rwlock));
    
    return rt;
}












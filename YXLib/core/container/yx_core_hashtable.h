//
//  yx_core_hashtable.h
//  YXLib
//
//  Created by LiuYuxi on 15/7/20.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_HASHTABLE__
#define _YX_CORE_HASHTABLE__

#include "../yx_core_global.h"

#include "../hash/yx_core_hash.h"
#include "yx_core_slistnode.h"


struct yx_core_hashtable_bucket{
    yx_core_hash key_hash;
    yx_value key;
    yx_value value;
    
    struct yx_core_slistnode node;
};

typedef struct yx_core_hashtable_bucket* yx_core_hashtable_bucket_ref;

typedef struct yx_core_hashtable {
    yx_rtti rtti;
    
    yx_allocator allocator;
    yx_allocator node_allocator;
    
    yx_size max_size;
    yx_size used;
    yx_core_slistnode_ref bucketArray;
    
    yx_compare_result (*compare_callback)(yx_value key1, yx_value key2); //use to compare two keys
    yx_core_hash (*hash_callback)(yx_value key); //use to get the key's hash value
    yx_value (*keycopy_callback)(yx_value key); //use to copy a key
    
    void (*del_callback)(yx_value key, yx_value value); //use to release data's buff
}yx_core_hashtable;

typedef struct yx_core_hashtable* yx_core_hashtable_ref;


yx_core_hashtable_ref yx_core_hashtable_create(yx_allocator allocator, yx_size size);
void yx_core_hashtable_init(yx_allocator allocator, yx_core_hashtable_ref hashtable, yx_size size);

void yx_core_hashtable_recycle(yx_core_hashtable_ref hashtable);
void yx_core_hashtable_destroy(yx_core_hashtable_ref* hashtable_ptr);



/*
 set callback function
 */
void yx_core_hashtable_set_callback_compare(yx_core_hashtable_ref hashtable, yx_compare_result (*compare_callback)(yx_value key1, yx_value key2));
void yx_core_hashtable_set_callback_del(yx_core_hashtable_ref hashtable, void (*del_callback)(yx_value key, yx_value value));
void yx_core_hashtable_set_callback_keycopy(yx_core_hashtable_ref hashtable, yx_value (*keycopy_callback)(yx_value key));
void yx_core_hashtable_set_callback_hash(yx_core_hashtable_ref hashtable, yx_core_hash (*hash_callback)(yx_value key));


/*
 property
 */
#define yx_core_hashtable_count(hashtable) ((hashtable)->used)
#define yx_core_hashtable_isEmpty(hashtable)  (0 != (hashtable)->used);
#define yx_core_hashtable_hashValue(hashtable, key) ((hashtable)->hash_callback(key))


/*
 set value for key
 */
//return yx_true if a value exist with the same key, else return yx_false
yx_bool yx_core_hashtable_set(yx_core_hashtable_ref hashtable, yx_value key, yx_value value);
yx_bool yx_core_hashtable_setWithHashKey(yx_core_hashtable_ref hashtable, yx_value key, yx_core_hash hashkey, yx_value value);


/*
 get value for key
 */
yx_value yx_core_hashtable_get(yx_core_hashtable_ref hashtable, yx_value key, yx_value defaultValue);
yx_value yx_core_hashtable_getWithHashkey(yx_core_hashtable_ref hashtable, yx_value key, yx_core_hash hashkey, yx_value defaultValue);

/*
 remove
 */
yx_bool yx_core_hashtable_remove(yx_core_hashtable_ref hashtable, yx_value key);
yx_bool yx_core_hashtable_removeWithHashkey(yx_core_hashtable_ref hashtable, yx_value key, yx_core_hash hashkey);


/*
 test
 */
yx_bool yx_core_hashtable_isExist(yx_core_hashtable_ref hashtable, yx_value key);
yx_bool yx_core_hashtable_isExistWithHashkey(yx_core_hashtable_ref hashtable, yx_value key, yx_core_hash hashkey);



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef yx_value yx_core_hashtable_remove_magicpt;

typedef struct yx_core_hashtable_breadth_cursor{
    yx_core_hashtable_ref hashtable;
    yx_core_hashtable_bucket_ref bucket;
    yx_core_hashtable_remove_magicpt magicpt;
    yx_long_index index;
}yx_core_hashtable_breadth_cursor;




void yx_core_hashtable_traversal_cursor_init(yx_core_hashtable_ref hashtable, yx_core_hashtable_breadth_cursor* cursor_ptr);
void yx_core_hashtable_traversal_cursor_next(yx_core_hashtable_breadth_cursor* cursor_ptr);
#define yx_core_hashtable_traversal_cursor_isEmpty(cursor_ptr)   (NULL == (cursor_ptr)->bucket)


#define yx_core_hashtable_loop(hashtable, cursor_ptr) \
    for(yx_core_hashtable_traversal_cursor_init((hashtable), (cursor_ptr)); \
        !yx_core_hashtable_traversal_cursor_isEmpty( (cursor_ptr) ); \
        yx_core_hashtable_traversal_cursor_next( (cursor_ptr) ) \
       )

/*
 use to remove the current node when traversal.
 */
void yx_core_hashtable_traversal_cursor_remove(yx_core_hashtable_breadth_cursor* cursor_ptr);



#endif /* defined(_YX_CORE_HASHTABLE__) */
























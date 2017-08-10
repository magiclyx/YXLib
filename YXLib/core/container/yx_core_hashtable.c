//
//  yx_core_hashtable.c
//  YXLib
//
//  Created by LiuYuxi on 15/7/20.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_hashtable.h"
#include "../allocator/yx_core_mempool_cell.h"
#include "../rttidef/yx_core_rttidef.h"
#include "yx_core_hashPrime.h"
#include "../debug/yx_core_assert.h"


#define yx_core_hashtable_invalidateNode ((yx_core_hashtable_node_ref)(-1))

static yx_compare_result _callback_dummy_compare(yx_value key1, yx_value key2); //use to compare two keys
static yx_core_hash _callback_dummy_hash(yx_value key); //use to get the key's hash value
static yx_value callback_dummy_keycopy(yx_value key); //use to copy a key


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma mark create_del

yx_core_hashtable_ref yx_core_hashtable_create(yx_allocator allocator, yx_size size)
{
    if (NULL == allocator)
        allocator = yx_get_allocator();
    
    yx_core_hashtable_ref hashtable = yx_allocator_alloc(allocator, sizeof(struct yx_core_hashtable));
    
    
    yx_core_hashtable_init(allocator, hashtable, size);
    
    
    return hashtable;
}


void yx_core_hashtable_init(yx_allocator allocator, yx_core_hashtable_ref hashtable, yx_size size)
{
    /*setup the rtti info*/
    hashtable->rtti = yx_core_rtti_hashtable;
    
    /*setup the allocator*/
    hashtable->allocator = allocator;
    
    /*setup the node allocator*/
    yx_os_rtti_if(allocator, yx_rtti_allocator_buf)
    {
        /*因为是bufpool, 所以这个cellMempool不会被释放*/
        hashtable->node_allocator = yx_cellMempool_create(allocator, sizeof(struct yx_core_hashtable_bucket));
    }
    else {
        hashtable->node_allocator = allocator;
    }

    
    /*setup the maxsize*/
    hashtable->max_size = yx_core_hash_primeSize(size * 2);
    if (0 == hashtable->max_size)
        hashtable->max_size = 1;
    
    /*setup the used*/
    hashtable->used = 0;
    
    /*setup the entry*/
    hashtable->bucketArray = (yx_core_slistnode_ref)yx_allocator_alloc(allocator, hashtable->max_size * sizeof(struct yx_core_hashtable_bucket));
    
    /*init the bucketArray*/
    for (size_t i=0; i < hashtable->max_size; i++) {
        yx_core_slistnode_Init(&(hashtable->bucketArray[i]));
    }
    
    
    /*setup the default compare callback*/
    hashtable->compare_callback = _callback_dummy_compare;
    hashtable->hash_callback = _callback_dummy_hash;
    hashtable->keycopy_callback = callback_dummy_keycopy;
    
    /*setup the default del callback*/
    hashtable->del_callback = NULL;
}

void yx_core_hashtable_destroy(yx_core_hashtable_ref* hashtable_ptr)
{
    yx_core_hashtable_ref hashtable = *hashtable_ptr;
    hashtable_ptr = NULL;
    
    yx_core_hashtable_recycle(hashtable);
    
    yx_allocator_free(hashtable->allocator, hashtable);
}


void yx_core_hashtable_recycle(yx_core_hashtable_ref hashtable)
{
    yx_os_rtti_notif(hashtable->allocator, yx_rtti_allocator_buf)
    {
        for (yx_long_index index = 0; hashtable->max_size; index++)
        {
            
            while( ! yx_core_slistnode_isEmpty(&(hashtable->bucketArray[index])) )
            {
                struct yx_core_hashtable_bucket* node = yx_core_slistnode_firstNodeOwner(&(hashtable->bucketArray[index]), struct yx_core_hashtable_bucket, node);
                
                if (NULL != hashtable->del_callback)
                {
                    hashtable->del_callback(node->key, node->value);
                }
                
                yx_core_slistnode_pop(&(hashtable->bucketArray[index]));
                
                yx_allocator_free(hashtable->node_allocator, node);

            }
            
        }
        
        yx_allocator_free(hashtable->allocator, hashtable->bucketArray);
    }
    else if(NULL != hashtable->del_callback)
    {
        /*buf memPool, 只有在有回调函数时，才回执行一次循环*/
        
        for (yx_long_index index = 0; hashtable->max_size; index++)
        {
            struct yx_core_hashtable_bucket* cursor;
            yx_core_slistnode_ownerForEach(cursor, &(hashtable->bucketArray[index]), node)
            {
                hashtable->del_callback(cursor->key, cursor->value);
            }

        }
    }

}



#pragma mark callback_setup
void yx_core_hashtable_set_callback_compare(yx_core_hashtable_ref hashtable, yx_compare_result (*compare_callback)(yx_value key1, yx_value key2))
{
    hashtable->compare_callback = compare_callback;
}

void yx_core_hashtable_set_callback_del(yx_core_hashtable_ref hashtable, void (*del_callback)(yx_value key, yx_value value))
{
    hashtable->del_callback = del_callback;
}

void yx_core_hashtable_set_callback_keycopy(yx_core_hashtable_ref hashtable, yx_value (*keycopy_callback)(yx_value key))
{
    hashtable->keycopy_callback = keycopy_callback;
}

void yx_core_hashtable_set_callback_hash(yx_core_hashtable_ref hashtable, yx_core_hash (*hash_callback)(yx_value key))
{
    hashtable->hash_callback = hash_callback;
}


#pragma mark operation
yx_bool yx_core_hashtable_set(yx_core_hashtable_ref hashtable, yx_value key, yx_value value)
{
    return yx_core_hashtable_setWithHashKey(hashtable, key, yx_core_hashtable_hashValue(hashtable, key), value);
}

yx_bool yx_core_hashtable_setWithHashKey(yx_core_hashtable_ref hashtable, yx_value key, yx_core_hash hashkey, yx_value value)
{
    yx_core_slistnode_ref bucketlist = &(hashtable->bucketArray[(hashkey % hashtable->max_size)]);
    
    
    struct yx_core_hashtable_bucket* cursor;
    struct yx_core_hashtable_bucket* found_bucket = NULL;
    yx_core_slistnode_ownerForEach(cursor, bucketlist, node)
    {
        if (cursor->key_hash == hashkey  &&  yx_compare_equal == hashtable->compare_callback(hashkey, key))
        {
            found_bucket = cursor;
            break;
        }
    }
    
    
    if (NULL != found_bucket)
    {
        found_bucket->value = value;
        found_bucket->key = hashtable->keycopy_callback(key);
        found_bucket->key_hash = hashkey;
        
        return yx_true;
    }
    else
    {
        yx_core_hashtable_bucket_ref newbucket = yx_allocator_alloc(hashtable->node_allocator, sizeof(struct yx_core_hashtable_bucket));
        newbucket->key_hash = hashkey;
        newbucket->key = hashtable->keycopy_callback(key);
        newbucket->value = value;
        
        yx_core_slistnode_push(&(newbucket->node), bucketlist);
        
        hashtable->used++;
        
        return yx_false;
    }
    
}



yx_value yx_core_hashtable_get(yx_core_hashtable_ref hashtable, yx_value key, yx_value defaultValue)
{
    return yx_core_hashtable_getWithHashkey(hashtable, key, yx_core_hashtable_hashValue(hashtable, key), defaultValue);
}

yx_value yx_core_hashtable_getWithHashkey(yx_core_hashtable_ref hashtable, yx_value key, yx_core_hash hashkey, yx_value defaultValue)
{
    
    yx_core_slistnode_ref bucketlist = &(hashtable->bucketArray[(hashkey % hashtable->max_size)]);
    
    
    struct yx_core_hashtable_bucket* cursor;
    yx_core_slistnode_ownerForEach(cursor, bucketlist, node)
    {
        if (cursor->key_hash == hashkey  &&  yx_compare_equal == hashtable->compare_callback(hashkey, key))
        {
            return cursor->value;
        }
    }
    
    
    return defaultValue;
}


yx_bool yx_core_hashtable_remove(yx_core_hashtable_ref hashtable, yx_value key)
{
    return yx_core_hashtable_removeWithHashkey(hashtable, key, yx_core_hashtable_hashValue(hashtable, key));
}

yx_bool yx_core_hashtable_removeWithHashkey(yx_core_hashtable_ref hashtable, yx_value key, yx_core_hash hashkey)
{
    
    yx_core_slistnode_ref bucketlist = &(hashtable->bucketArray[(hashkey % hashtable->max_size)]);
    
    
    yx_core_slistnode_ref prev_node = bucketlist;
    yx_core_slistnode_ref current_node = prev_node->next;
    while (current_node != bucketlist)
    {
        struct yx_core_hashtable_bucket* cursor = yx_core_slistnode_owner(current_node, struct yx_core_hashtable_bucket, node);
        
        if (cursor->key_hash == hashkey  &&  yx_compare_equal == hashtable->compare_callback(cursor->key, key))
        {
            __yx_core_slistnode_del(prev_node);
            
            if (NULL != hashtable->del_callback)
                hashtable->del_callback(cursor->key, cursor->value);
            
            yx_allocator_free(hashtable->node_allocator, cursor);
            
            
            hashtable->used--;
            
            return yx_true;
        }
        else
        {
            prev_node = prev_node->next;
        }
        
        current_node = prev_node->next;
    }
    
    
    return yx_false;
}


yx_bool yx_core_hashtable_isExist(yx_core_hashtable_ref hashtable, yx_value key)
{
    return yx_core_hashtable_isExistWithHashkey(hashtable, key, yx_core_hashtable_hashValue(hashtable, key));
}
yx_bool yx_core_hashtable_isExistWithHashkey(yx_core_hashtable_ref hashtable, yx_value key, yx_core_hash hashkey)
{
    yx_core_slistnode_ref bucketlist = &(hashtable->bucketArray[(hashkey % hashtable->max_size)]);
    
    struct yx_core_hashtable_bucket* cursor;
    yx_core_slistnode_ownerForEach(cursor, bucketlist, node)
    {
        if (cursor->key_hash == hashkey  &&  yx_compare_equal == hashtable->compare_callback(hashkey, key))
        {
            return yx_true;
        }
    }
    
    
    return yx_false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#define yx_core_hashtable_traversal_removed_flag 1
#define yx_core_hashtable_traversal_unremoved_flag 0


#define yx_core_hashtable_traversal_magicpt_pack(node, remove_flag) \
    ((yx_core_hashtable_remove_magicpt)(node) | (yx_core_hashtable_remove_magicpt)(remove_flag))

#define yx_core_hashtable_traversal_node( magicpt ) \
    ( (yx_core_slistnode_ref) ( (magicpt) & ~((yx_core_hashtable_remove_magicpt)3) ) )


#define yx_core_hashtable_traversal_isRemoved( magicpt ) \
    ( (magicpt) & yx_core_hashtable_traversal_removed_flag )


void yx_core_hashtable_traversal_cursor_init(yx_core_hashtable_ref hashtable, yx_core_hashtable_breadth_cursor* cursor_ptr)
{
    YX_ASSERT(NULL != hashtable);
    YX_ASSERT(NULL != cursor_ptr);
    
    cursor_ptr->hashtable = hashtable;
    cursor_ptr->index = (yx_long_index)(-1);
    cursor_ptr->bucket = NULL;
    
    for (yx_long_index index = 0; index < hashtable->max_size; index++)
    {
        if (! yx_core_slistnode_isEmpty( &(hashtable->bucketArray[index]) ) )
        {
            cursor_ptr->index = index;
            
            cursor_ptr->bucket = yx_core_slistnode_firstNodeOwner(
                                                                  &(hashtable->bucketArray[index]),
                                                                  struct yx_core_hashtable_bucket,
                                                                  node
                                                                  );
            
            cursor_ptr->magicpt = yx_core_hashtable_traversal_magicpt_pack(
                                                                           &(hashtable->bucketArray[index]),
                                                                           yx_core_hashtable_traversal_unremoved_flag);
            
            break;
        }
    }
    
}

void yx_core_hashtable_traversal_cursor_next(yx_core_hashtable_breadth_cursor* cursor_ptr)
{
    YX_ASSERT(NULL != cursor_ptr);
    YX_ASSERT(NULL != cursor_ptr->hashtable);
    
    /*not invalidate bucket*/
    YX_ASSERT(NULL != cursor_ptr->bucket);
    
    
    
    if (yx_core_hashtable_traversal_isRemoved(cursor_ptr->magicpt))
        return;
    
    
    yx_core_hashtable_ref hashtable = cursor_ptr->hashtable;

    if ( yx_core_slistnode_isLast( &(cursor_ptr->bucket->node), &(cursor_ptr->hashtable->bucketArray[cursor_ptr->index]) ) )
    {
        yx_long_index index = cursor_ptr->index + 1;
        
        cursor_ptr->index = (yx_long_index)(-1);
        cursor_ptr->bucket = NULL;
        cursor_ptr->magicpt = 0;
        
        for (; index < hashtable->max_size; index++)
        {
            if (! yx_core_slistnode_isEmpty( &(hashtable->bucketArray[index]) ) )
            {
                cursor_ptr->index = index;
                
                cursor_ptr->bucket = yx_core_slistnode_firstNodeOwner(
                                                                      &(hashtable->bucketArray[index]),
                                                                      struct yx_core_hashtable_bucket,
                                                                      node
                                                                      );
                
                cursor_ptr->magicpt = yx_core_hashtable_traversal_magicpt_pack(
                                                                               &(hashtable->bucketArray[index]),
                                                                               yx_core_hashtable_traversal_unremoved_flag);

                
                break;
            }

        }
    }
    else
    {
        cursor_ptr->magicpt = yx_core_hashtable_traversal_magicpt_pack(
                                                                       &(cursor_ptr->bucket->node),
                                                                       yx_core_hashtable_traversal_unremoved_flag);
        cursor_ptr->bucket = yx_core_slistnode_nextNodeOwner(&(cursor_ptr->bucket->node), struct yx_core_hashtable_bucket, node);
    }
}

void yx_core_hashtable_traversal_cursor_remove(yx_core_hashtable_breadth_cursor* cursor_ptr)
{
    YX_ASSERT(NULL != cursor_ptr);
    YX_ASSERT(NULL != cursor_ptr->hashtable);
    
    yx_core_hashtable_ref hashtable = cursor_ptr->hashtable;

    
    /*能够删除node, 此时slist必定不是空*/
    YX_ASSERT(! yx_core_slistnode_isEmpty( &( hashtable->bucketArray[cursor_ptr->index]) ) );
    
    
    yx_core_slistnode_ref pre_node = yx_core_hashtable_traversal_node(cursor_ptr->magicpt);
    __yx_core_slistnode_del(pre_node);
    hashtable->used--;
    
    if ( yx_core_slistnode_isLast( pre_node, &(cursor_ptr->hashtable->bucketArray[cursor_ptr->index]) ) )
    {
        yx_long_index index = cursor_ptr->index + 1;

        cursor_ptr->index = (yx_long_index)(-1);
        cursor_ptr->bucket = NULL;
        cursor_ptr->magicpt = 0;
        
        for (; index < hashtable->max_size; index++)
        {
            if (! yx_core_slistnode_isEmpty( &(hashtable->bucketArray[index]) ) )
            {
                cursor_ptr->index = index;
                
                cursor_ptr->bucket = yx_core_slistnode_firstNodeOwner(
                                                                      &(hashtable->bucketArray[index]),
                                                                      struct yx_core_hashtable_bucket,
                                                                      node
                                                                      );
                
                cursor_ptr->magicpt = yx_core_hashtable_traversal_magicpt_pack(
                                                                               &(hashtable->bucketArray[index]),
                                                                               yx_core_hashtable_traversal_unremoved_flag);
                
                
                break;
            }
            
        }

    }
    else
    {
        cursor_ptr->bucket = yx_core_slistnode_nextNodeOwner(pre_node->next, struct yx_core_hashtable_bucket, node);
    }
    
    yx_core_hashtable_traversal_magicpt_pack(pre_node, yx_core_hashtable_traversal_removed_flag);

}




////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma mark private
static yx_compare_result _callback_dummy_compare(yx_value key1, yx_value key2) //use to compare two keys
{
    if (key1 > key2)
    {
        return yx_compare_larger;
    }
    else if(key1 < key2)
    {
        return yx_compare_smaller;
    }
    else
    {
        return yx_compare_equal;
    }
}

static yx_core_hash _callback_dummy_hash(yx_value key) //use to get the key's hash value
{
    return (yx_core_hash)key;
}

static yx_value callback_dummy_keycopy(yx_value key) //use to copy a key
{
    return key;
}









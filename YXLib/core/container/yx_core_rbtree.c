//
//  yx_core_rbtree.c
//  YXLib
//
//  Created by LiuYuxi on 15/6/26.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_rbtree.h"

#include "../rttidef/yx_core_rttidef.h"
#include "../allocator/yx_core_mempool_cell.h"
#include "../debug/yx_core_assert.h"
#include "../allocator/yx_core_mempool_buf.h"


////////////////////////////////////////////////////////////////////////////////////////////////////

static yx_compare_result _callback_dummy_compare(yx_value key1, yx_value key2); //use to compare two keys
static yx_core_hash _callback_dummy_hash(yx_value key); //use to get the key's hash value
static yx_value _callback_dummy_keycopy(yx_value key); //use to copy a key


static yx_bool _new_value_for_key(yx_core_rbtree_ref rbtree, yx_value data, yx_value key, yx_bool overwrite);

static struct yx_core_rbnode_wrapper* _nodeWrapper_for_key(yx_core_rbtree_ref rbtree, yx_value key);

static void _yx_core_rbnode_resetTree(yx_core_rbtree_ref rbtree);


////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 create & destroy
 */
yx_core_rbtree_ref yx_core_rbtree_create(yx_allocator allocator)
{
    if (NULL == allocator)
        allocator = yx_get_allocator();
    
    yx_core_rbtree_ref tree = (yx_core_rbtree_ref)yx_allocator_alloc(allocator, sizeof(struct yx_core_rbtree));
    

    yx_core_rbtree_init(allocator, tree);
    
    
    return tree;
}

void yx_core_rbtree_init(yx_allocator allocator, yx_core_rbtree_ref tree)
{
    /*setup the rtti info*/
    tree->rtti = yx_core_rtti_rbtree;
    
    /*setup the allocator*/
    tree->allocator = allocator;
    
    /*setup the node allocator*/
    yx_os_rtti_if(allocator, yx_rtti_allocator_buf) {
        /*因为是bufpool, 所以这个cellMempool不会被释放*/
        tree->node_allocator = yx_cellMempool_create(allocator, sizeof(struct yx_core_rbnode_wrapper));
    }
    else {
        tree->node_allocator = allocator;
    }
    
    /*init the rbtree root node*/
    tree->root = YX_CORE_RBROOT;
    
    /*init the count*/
    tree->count = 0;
    
    /*setup the default compare callback*/
    tree->compare_callback = _callback_dummy_compare;
    
    /*setup the hash callback*/
    tree->hash_callback = _callback_dummy_hash;
    tree->keycopy_callback = _callback_dummy_keycopy;
    
    /*setup the default del callback*/
    tree->del_callback = NULL;
    
}


void yx_core_rbtree_destroy(yx_core_rbtree_ref* tree_ptr)
{
    yx_core_rbtree_ref tree = *tree_ptr;
    *tree_ptr = NULL;
    
    yx_core_rbtree_recycle(tree);
    
    yx_allocator_free(tree->allocator, tree);
}

void yx_core_rbtree_recycle(yx_core_rbtree_ref tree)
{
    yx_os_rtti_notif(tree->allocator, yx_core_rtti_rbtree)
    {
        _yx_core_rbnode_resetTree(tree);
    }
    else if(NULL != tree->del_callback)
    {
        /*buf memPool, 只有在有回调函数时，才回执行一次循环*/
        
        yx_core_rbnode_wrapper_ref node;
        yx_core_rbtree_forEach(&(tree->root), node) {
            tree->del_callback(node->data);
        }
    }

}



void yx_core_rbtree_set_callback_compare(yx_core_rbtree_ref rbtree, yx_compare_result (*compare_callback)(yx_value key1, yx_value key2))
{
    rbtree->compare_callback = compare_callback;
}

void yx_core_rbtree_set_callback_hash(yx_core_rbtree_ref rbtree, yx_core_hash (*hash_callback)(yx_value key))
{
    rbtree->hash_callback = hash_callback;
}


void yx_core_rbtree_set_callback_del(yx_core_rbtree_ref rbtree, void (*del_callback)(yx_value data))
{
    rbtree->del_callback = del_callback;
}



/*set*/
void yx_core_rbtree_setDataForKey(yx_core_rbtree_ref rbtree, yx_value data, yx_value key)
{
    _new_value_for_key(rbtree, data, key, yx_true);
}

yx_bool yx_core_rbtree_addDataForKey(yx_core_rbtree_ref rbtree, yx_value data, yx_value key)
{
    return _new_value_for_key(rbtree, data, key, yx_false);
}



/*get*/
yx_value yx_core_rbtree_dataForKey(const yx_core_rbtree_ref rbtree, yx_value key, yx_value defalutData)
{
    
    const struct yx_core_rbnode_wrapper* node_wrapper = _nodeWrapper_for_key(rbtree, key);
    if (NULL != node_wrapper)
    {
        return node_wrapper->data;
    }
    else
    {
        return defalutData;
    }
}

yx_bool yx_core_rbtree_isKeyExist(const yx_core_rbtree_ref rbtree, yx_value key)
{
    const struct yx_core_rbnode_wrapper* node_wrapper = _nodeWrapper_for_key(rbtree, key);
    
    if (NULL != node_wrapper)
        return yx_true;
    else
        return yx_false;
}


/*del*/
void yx_core_rbtree_removeDataForKey(yx_core_rbtree_ref rbtree, const yx_value key)
{
    struct yx_core_rbnode_wrapper* node_wrapper = _nodeWrapper_for_key(rbtree, key);
    if (NULL != node_wrapper)
    {
        yx_core_rbnode_erase(&(node_wrapper->node), &(rbtree->root));
        rbtree->count--;
        
        if (NULL != rbtree->del_callback)
        {
            rbtree->del_callback(node_wrapper->data);
        }

        yx_allocator_free(rbtree->node_allocator, node_wrapper);
    }
    
}

void yx_core_rbtree_removeAllData(yx_core_rbtree_ref rbtree)
{
    _yx_core_rbnode_resetTree(rbtree);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////



void yx_core_rbtree_breadthFirstTraversal(const yx_core_rbtree_ref rbtree, yx_value bindingData, void (*traversal_callback)(yx_value key, yx_value data, yx_value bindingData))
{
    YX_ASSERT(NULL != rbtree);
    
    if (NULL == traversal_callback)
        return;
    
    yx_allocator bufAllocator = yx_bufMempool_create_std();
    
    /*init hte queue*/
    struct yx_core_queue queue;
    yx_core_queue_init(bufAllocator, &queue);
    
    
    /*push the first node*/
    yx_core_rbnode_ref cursor = rbtree->root.yx_rbnode;
    if (NULL != cursor)
        yx_core_queue_push(&queue, (yx_value)cursor);
    
    
    while (yx_false == yx_core_queue_isEmpty(&queue))
    {
        cursor = (struct yx_core_rbnode*)yx_core_queue_first(&queue);
        yx_core_queue_pop(&queue);
        
        const yx_core_rbnode_wrapper_ref wrapper = yx_core_rbnode_owner(cursor, struct yx_core_rbnode_wrapper, node);
        
        traversal_callback(wrapper->key, wrapper->data, bindingData);
        
        
        yx_core_rbnode_ref tmp_node = cursor->rb_left;
        if (NULL != tmp_node)
            yx_core_queue_push(&queue, (yx_value)tmp_node);
        
        
        tmp_node = cursor->rb_right;
        if (NULL != tmp_node)
            yx_core_queue_push(&queue, (yx_value)tmp_node);
    }
    
    
    //buffPool 不用回收
    //yx_core_queue_recycle(&queue);
    
    yx_bufMempool_destroy(&bufAllocator);
}

void yx_core_rbtree_inorderTraversal(const yx_core_rbtree_ref rbtree, yx_value bindingData, void (*traversal_callback)(yx_value key, yx_value data, yx_value bindingData))
{
    YX_ASSERT(NULL != rbtree);
    
    if (NULL == traversal_callback)
        return;
    
    yx_core_rbnode_ref cursor = yx_core_rbnode_firstNode(&(rbtree->root));
    while (NULL != cursor)
    {
        const yx_core_rbnode_wrapper_ref wrapper = yx_core_rbnode_owner(cursor, struct yx_core_rbnode_wrapper, node);
        
        traversal_callback(wrapper->key, wrapper->data, bindingData);
        
        cursor = yx_core_rbnode_nextNode(cursor);
    }
}

void yx_core_rbtree_inorderTraversal_reverse(const yx_core_rbtree_ref rbtree, yx_value bindingData, void (*traversal_callback)(yx_value key, yx_value data, yx_value bindingData))
{
    YX_ASSERT(NULL != rbtree);
    
    if (NULL == traversal_callback)
        return;
    
    yx_core_rbnode_ref cursor = yx_core_rbnode_lastNode(&(rbtree->root));
    while (NULL != cursor)
    {
        const yx_core_rbnode_wrapper_ref wrapper = yx_core_rbnode_owner(cursor, struct yx_core_rbnode_wrapper, node);
        
        traversal_callback(wrapper->key, wrapper->data, bindingData);
        
        cursor = yx_core_rbnode_prevNode(cursor);
    }
}



void yx_core_rbtree_postorderTraversal(const yx_core_rbtree_ref rbtree, yx_value bindingData, void (*traversal_callback)(yx_value key, yx_value data, yx_value bindingData))
{
    YX_ASSERT(NULL != rbtree);
    
    if (NULL == traversal_callback)
        return;
    
    yx_core_rbnode_ref cursor = yx_core_rbnode_postorder_firstNode(&(rbtree->root));
    while (NULL != cursor)
    {
        const yx_core_rbnode_wrapper_ref wrapper = yx_core_rbnode_owner(cursor, struct yx_core_rbnode_wrapper, node);
        
        traversal_callback(wrapper->key, wrapper->data, bindingData);
        
        cursor = yx_core_rbnode_postorder_nextNode(cursor);
    }

}



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

static yx_value _callback_dummy_keycopy(yx_value key) //use to copy a key
{
    return key;
}



static yx_bool _new_value_for_key(yx_core_rbtree_ref rbtree, yx_value data, yx_value key, yx_bool overwrite)
{
    //    struct yx_core_rbnode **tmp = &(root->yx_rbnode), *parent = NULL;
    struct yx_core_rbnode** tmp = &(rbtree->root.yx_rbnode);
    struct yx_core_rbnode* parent = NULL;
    
    yx_core_hash key_hash = rbtree->hash_callback(key);
    
    while (*tmp)
    {
        struct yx_core_rbnode_wrapper* this = yx_core_rbnode_owner(*tmp, struct yx_core_rbnode_wrapper, node);
        
        parent = *tmp;
        
        
        
        
        yx_compare_result compare_result;
        
        if (key_hash > this->key_hash){
            compare_result = yx_compare_larger;
        }
        else if(key_hash < this->key_hash){
            compare_result = yx_compare_smaller;
        }
        else {
            //yx_compare_equal;
            compare_result = rbtree->compare_callback(key, this->key);
        }

        
        
        if (yx_compare_smaller == compare_result){
            tmp = &((*tmp)->rb_left);
        }
        else if(yx_compare_larger == compare_result){
            tmp = &((*tmp)->rb_right);
        }
        else
        {
            if (yx_true == overwrite)
            {
                if (NULL != rbtree->del_callback)
                {
                    rbtree->del_callback(this->data);
                }
                this->data = data;
                this->key = rbtree->keycopy_callback(key);
                this->key_hash = key_hash;
                return yx_true;
            }
            else
            {
                return yx_false;
            }
        }
        
    }
    
    
    struct yx_core_rbnode_wrapper* newNodeWrapper = yx_allocator_alloc(rbtree->node_allocator, sizeof(struct yx_core_rbnode_wrapper));
    newNodeWrapper->data = data;
    newNodeWrapper->key = rbtree->keycopy_callback(key);
    newNodeWrapper->key_hash = key_hash;
    
    
    yx_core_rbnode_insert(&(newNodeWrapper->node), parent, &(rbtree->root), tmp);
    rbtree->count++;
    
    return yx_true;
}

static struct yx_core_rbnode_wrapper* _nodeWrapper_for_key(yx_core_rbtree_ref rbtree, yx_value key)
{
    struct yx_core_rbnode* node = rbtree->root.yx_rbnode;
    
    yx_core_hash keyhash = rbtree->hash_callback(key);
    
    while (node) {
        struct yx_core_rbnode_wrapper* node_wrapper = yx_core_rbnode_owner(node, struct yx_core_rbnode_wrapper, node);
        
        
        yx_compare_result compare_result;
        
        if (keyhash > node_wrapper->key_hash){
            compare_result = yx_compare_larger;
        }
        else if(keyhash < node_wrapper->key_hash){
            compare_result = yx_compare_smaller;
        }
        else{
            compare_result = rbtree->compare_callback(key, node_wrapper->key);
        }
        
        
        
        if (yx_compare_smaller == compare_result) {
            node = node->rb_left;
        }
        else if(yx_compare_larger == compare_result) {
            node = node->rb_right;
        }
        else {
            return node_wrapper;
        }
    }
    
    
    return NULL;
}

static void _yx_core_rbnode_resetTree(yx_core_rbtree_ref rbtree)
{
    /*
     Breadth-first traversal to delete the nodes
     
     Fast and need no more memory, However, in the process of traversing, the tree will be destroyed.
     */
    
    /*这里用magicpt存储一个node指针*/
    YX_ASSERT(sizeof(struct yx_core_slistnode) <= sizeof(yx_core_rbnode_magicpt));
    
    if (! yx_core_rbnode_isTreeEmpty(&(rbtree->root)) )
    {
        /*init a queue*/
        struct yx_core_queuehead queue;
        yx_core_queuenode_Init(&queue);
        
        
        yx_core_rbnode_ref rbnode;
        yx_core_slistnode_ref node = (yx_core_slistnode_ref)(&(rbtree->root.yx_rbnode->__rb_parent_color));
        
        yx_core_queuenode_push(node, &queue);
        rbtree->root = YX_CORE_RBROOT;
        
        while (!yx_core_queuenode_isEmpty(&queue))
        {
            /*get a list node from queue and forece cast it to a pointer with yx_core_rbnode_magicpt type*/
            const yx_core_rbnode_magicpt* magicpt = (yx_core_rbnode_magicpt*)yx_core_queuenode_pop(&queue);
            
            /*get the rbnode pointer from the yx_core_rbnode_magicpt pointer*/
            rbnode = YX_CONTAINER_OF(magicpt, struct yx_core_rbnode, __rb_parent_color);
            /*get the rbnode_wrapper pointer from the rbnode pointer*/
            struct yx_core_rbnode_wrapper* node_wrapper = yx_core_rbnode_owner(rbnode, struct yx_core_rbnode_wrapper, node);
            
            /*push the rbnode's left child into the queue*/
            if (NULL != rbnode->rb_left)
            {
                node = (yx_core_slistnode_ref)(&(rbnode->rb_left->__rb_parent_color));
                yx_core_queuenode_push(node, &queue);
            }
            
            /*push the rbnode's right child into the queue*/
            if (NULL != rbnode->rb_right)
            {
                node = (yx_core_slistnode_ref)(&(rbnode->rb_right->__rb_parent_color));
                yx_core_queuenode_push(node, &queue);
            }
            
            /*call the del_callback function*/
            if (NULL != rbtree->del_callback)
                rbtree->del_callback(node_wrapper->data);
            
            /*free the node*/
            yx_allocator_free(rbtree->node_allocator, node_wrapper);
        }
        
        
        /*reset the tree count to 0*/
        rbtree->count = 0;
        
    }
    
}



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
void yx_core_rbtree_traversal_cursor_init(const yx_core_rbtree_ref rbtree, yx_core_rbtree_breadth_cursor* cursor)
{
    YX_ASSERT(NULL != rbtree);
    YX_ASSERT(NULL != cursor);
    
    
    cursor->bufAllocator = yx_bufMempool_create(rbtree->allocator, YX_BUFPOOL_STD_BUFFLEN);
    
    yx_core_queue_init(cursor->bufAllocator, &(cursor->queue));
    
    
    const yx_core_rbnode_ref node = rbtree->root.yx_rbnode;
    if (NULL != node)
    {
        cursor->node = yx_core_rbnode_owner(node, struct yx_core_rbnode_wrapper, node);
    }
    else
    {
        cursor->node = NULL;
    }
}


void yx_core_rbtree_traversal_cursor_recycle(yx_core_rbtree_breadth_cursor* cursor)
{
    YX_ASSERT(NULL != cursor);
    
    //bufPool 不用回收
    //yx_core_queue_recycle(&(cursor->queue));
    
    yx_bufMempool_destroy(&(cursor->bufAllocator));
    
#if YX_DEBUG
    cursor->bufAllocator = NULL;
    cursor->node = NULL;
#endif
}



void yx_core_rbtree_traversal_cursor_next(yx_core_rbtree_breadth_cursor* cursor)
{
    
    YX_ASSERT(NULL != cursor);
    
    if (NULL == cursor->node)
        return;
    
    
    if (yx_false == yx_core_queue_isEmpty(&(cursor->queue)))
    {
        const yx_core_rbnode_ref node = (yx_core_rbnode_ref)yx_core_queue_first(&(cursor->queue));
        yx_core_queue_pop(&(cursor->queue));
        
        cursor->node = yx_core_rbnode_owner(node, struct yx_core_rbnode_wrapper, node);
        
        
        yx_core_rbnode_ref tmp_node = node->rb_left;
        if (NULL != tmp_node)
            yx_core_queue_push(&(cursor->queue), (yx_value)tmp_node);
        
        
        tmp_node = node->rb_right;
        if (NULL != tmp_node)
            yx_core_queue_push(&(cursor->queue), (yx_value)tmp_node);
    }
    else
    {
        cursor->node = NULL;
    }
    
}



yx_core_rbnode_wrapper_ref yx_core_rbtree_inorder_firstNode(const yx_core_rbtree_ref rbtree)
{
    const yx_core_rbnode_ref node = yx_core_rbnode_firstNode(&(rbtree->root));
    
    if (NULL != node)
    {
        return yx_core_rbnode_owner(node, struct yx_core_rbnode_wrapper, node);
    }
    else
    {
        return NULL;
    }
}


yx_core_rbnode_wrapper_ref yx_core_rbtree_inorder_lastNode(const yx_core_rbtree_ref rbtree)
{
    const yx_core_rbnode_ref node = yx_core_rbnode_lastNode(&(rbtree->root));
    
    if (NULL != node)
    {
        return yx_core_rbnode_owner(node, struct yx_core_rbnode_wrapper, node);
    }
    else
    {
        return NULL;
    }

}

yx_core_rbnode_wrapper_ref yx_core_rbtree_inorder_nextNode(yx_core_rbnode_wrapper_ref node_wraper)
{
    const yx_core_rbnode_ref node = yx_core_rbnode_nextNode(&(node_wraper->node));
    
    if (NULL != node)
    {
        return yx_core_rbnode_owner(node, struct yx_core_rbnode_wrapper, node);
    }
    else
    {
        return NULL;
    }
}

yx_core_rbnode_wrapper_ref yx_core_rbtree_inorder_prevNode(yx_core_rbnode_wrapper_ref node_wraper)
{
    const yx_core_rbnode_ref node = yx_core_rbnode_prevNode(&(node_wraper->node));
    
    if (NULL != node)
    {
        return yx_core_rbnode_owner(node, struct yx_core_rbnode_wrapper, node);
    }
    else
    {
        return NULL;
    }

}

//------------------------------------------------------------------------------------
//postorder traversal
yx_core_rbnode_wrapper_ref yx_core_rbtree_postorder_firstNode(const yx_core_rbtree_ref rbtree)
{
    const yx_core_rbnode_ref node = yx_core_rbnode_postorder_firstNode(&(rbtree->root));
    
    if (NULL != node)
    {
        return yx_core_rbnode_owner(node, struct yx_core_rbnode_wrapper, node);
    }
    else
    {
        return NULL;
    }
}

yx_core_rbnode_wrapper_ref yx_core_rbtree_postorder_nextNode(yx_core_rbnode_wrapper_ref node_wraper)
{
    const yx_core_rbnode_ref node = yx_core_rbnode_postorder_nextNode(&(node_wraper->node));
    if (NULL != node)
    {
        return yx_core_rbnode_owner(node, struct yx_core_rbnode_wrapper, node);
    }
    else
    {
        return NULL;
    }
}



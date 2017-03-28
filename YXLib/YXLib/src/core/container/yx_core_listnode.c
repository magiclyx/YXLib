//
//  yx_core_list.c
//  YXLib
//
//  Created by LiuYuxi on 15/4/14.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_listnode.h"


yx_core_listnode_ref yx_core_listnode_midNode(const yx_core_listnode_ref sentinel_node)
{
    yx_core_listnode_ref middle_node;
    yx_core_listnode_ref last_node;
    
    middle_node = yx_core_listnode_firstNode(sentinel_node);
    
    if (middle_node == yx_core_listnode_lastNode(sentinel_node)) {
        return middle_node;
    }
    
    
    last_node = yx_core_listnode_firstNode(sentinel_node);
    
    for ( ;; ) {
        middle_node = yx_core_listnode_nextNode(middle_node);
        
        last_node = yx_core_listnode_nextNode(last_node);
        
        if (last_node == yx_core_listnode_lastNode(sentinel_node)) {
            return middle_node;
        }
        
        last_node = yx_core_listnode_nextNode(last_node);
        
        if (last_node == yx_core_listnode_lastNode(sentinel_node)) {
            return middle_node;
        }
    }
}


void yx_core_list_sort(yx_core_listnode_ref sentinel_node,
                       yx_int (*cmp)(const yx_core_listnode_ref node1, const yx_core_listnode_ref node2))
{
    yx_core_listnode_ref q;
    yx_core_listnode_ref prev;
    yx_core_listnode_ref next;
    
    q = yx_core_listnode_firstNode(sentinel_node);
    
    if (q == yx_core_listnode_lastNode(sentinel_node)) {
        return;
    }
    
    for (q = yx_core_listnode_nextNode(sentinel_node); q != sentinel_node; q = next) {
        
        prev = yx_core_listnode_prevNode(sentinel_node);
        next = yx_core_listnode_nextNode(sentinel_node);
        
        
        yx_core_listnode_del(q);
        
        
        do {
            if (cmp(prev, q) <= 0) {
                break;
            }
            
            prev = yx_core_listnode_prevNode(prev);
            
        } while (prev != sentinel_node);
        
        yx_core_listnode_insertAfterNode(prev, q);
    }
}


//
//  yx_core_rbnode.h
//  YXLib
//
//  Created by LiuYuxi on 15/6/25.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#ifndef __YX_CORE_RBNODE_H_
#define __YX_CORE_RBNODE_H_

#include "../yx_core_global.h"

#define YX_CORE_RBNODE_RED    0
#define	YX_CORE_RBNODE_BLACK  1

typedef yx_value yx_core_rbnode_magicpt;

struct yx_core_rbnode {
    yx_core_rbnode_magicpt __rb_parent_color;
    struct yx_core_rbnode *rb_right;
    struct yx_core_rbnode *rb_left;
};

typedef struct yx_core_rbnode* yx_core_rbnode_ref;

struct yx_core_rbroot {
    struct yx_core_rbnode *yx_rbnode;
};


#define YX_CORE_RBROOT	(struct yx_core_rbroot) { NULL, }


/*magicpt - unpack*/
#define yx_core_rbnode_magicpt_parent( magicpt ) \
    ( (struct yx_core_rbnode *) ( (magicpt) & ~((yx_core_rbnode_magicpt)3) ) )

#define yx_core_rbnode_magicpt_color( magicpt ) \
    ( (magicpt) & 1 )


#define yx_core_rbnode_magicpt_isBlack(magicpt) ((magicpt) & (YX_CORE_RBNODE_BLACK))
#define yx_core_rbnode_magicpt_isRed(magicpt)   (!(yx_core_rbnode_magicpt_isBlack(magicpt)))


/*magicpt - pack*/
#define yx_core_rbnode_magicpt_pack(parentNode, color) \
    ((yx_core_rbnode_magicpt)(parentNode) | (yx_core_rbnode_magicpt)(color))

#define yx_core_rbnode_magicpt_pack_parent( magicpt, parent ) \
    ( (yx_core_rbnode_magicpt)(yx_core_rbnode_magicpt_color(magicpt)) | (yx_core_rbnode_magicpt)(parent))

//#define yx_core_rbnode_magicpt_pack_color(magicpt, color) \
//    ( (yx_core_rbnode_magicpt)(yx_core_rbnode_magicpt_parent(magicpt)) | (yx_core_rbnode_magicpt)(color))

#define yx_core_rbnode_magicpt_pack_red(magicpt) \
    ( (yx_core_rbnode_magicpt)(yx_core_rbnode_magicpt_parent(magicpt)) )

#define yx_core_rbnode_magicpt_pack_black(magicpt) \
    ( (magicpt) | (yx_core_rbnode_magicpt)(YX_CORE_RBNODE_BLACK))




/*get node state*/
#define yx_core_rbnode_color(node)        yx_core_rbnode_magicpt_color((node)->__rb_parent_color)
#define yx_core_rbnode_isRed(node)      yx_core_rbnode_magicpt_isRed((node)->__rb_parent_color)
#define yx_core_rbnode_isBlack(node)    yx_core_rbnode_magicpt_isBlack((node)->__rb_parent_color)

#define yx_core_rbnode_isTreeEmpty(root) ( NULL == (root)->yx_rbnode )

/* 'wild' nodes are nodes that known not to be inserted in an rbtree.*/
#define yx_core_rbnode_isWild(node) \
    ((node)->__rb_parent_color == (yx_core_rbnode_magicpt)(node))


/*set node state*/
#define yx_core_rbnode_setWild(node) \
    ((node)->__rb_parent_color = (yx_core_rbnode_magicpt)(node))

#define yx_core_rbnode_setBlack(node) \
    yx_core_rbnode_magicpt_pack_black((node)->__rb_parent_color)

#define yx_core_rbnode_setRed(node) \
    yx_core_rbnode_magicpt_pack_red((node)->__rb_parent_color)

#define yx_core_rbnode_setParent(node, parent) \
    yx_core_rbnode_magicpt_pack_parent((node)->__rb_parent_color, parent)

#define yx_core_rbnode_setParentAndColor(node, parent, color) \
    ( (node)->__rb_parent_color = yx_core_rbnode_magicpt_pack(parent, color) )


/*insert and erase*/
void yx_core_rbnode_insert(struct yx_core_rbnode* node, struct yx_core_rbnode* parent, struct yx_core_rbroot* root, struct yx_core_rbnode** rb_link);

void yx_core_rbnode_erase(struct yx_core_rbnode* node, struct yx_core_rbroot* root);



/* Find logical next and previous nodes in a tree */
#define yx_core_rbnode_parentNode(node) \
    yx_core_rbnode_magicpt_parent( (node)->__rb_parent_color )
struct yx_core_rbnode* yx_core_rbnode_nextNode(const struct yx_core_rbnode* node);
struct yx_core_rbnode* yx_core_rbnode_prevNode(const struct yx_core_rbnode* node);
struct yx_core_rbnode* yx_core_rbnode_firstNode(const struct yx_core_rbroot* root);
struct yx_core_rbnode* yx_core_rbnode_lastNode(const struct yx_core_rbroot* root);


/* Postorder iteration - always visit the parent after its children */
struct yx_core_rbnode* yx_core_rbnode_postorder_firstNode(const struct yx_core_rbroot* root);
struct yx_core_rbnode* yx_core_rbnode_postorder_nextNode(const struct yx_core_rbnode* node);



/* Fast replacement of a single node without remove/rebalance/add/rebalance */
void rb_replace_node(struct yx_core_rbnode *victim, struct yx_core_rbnode *newone,
                            struct yx_core_rbroot *root);


#define yx_core_rbnode_owner(node, struct_type, node_name)  YX_CONTAINER_OF(node, struct_type, node_name)


#define yx_core_rbnode_safeOwner(ptr, type, member) \
    ({ typeof(ptr) ____ptr = (ptr); \
    ____ptr ? rb_entry(____ptr, type, member) : NULL; \
    })


/**
 * yx_core_rbnode_postorder_forEachSafe - iterate over rb_root in post order of
 * given type safe against removal of yx_rbnode entry
 *
 * @cursor:	the 'type *' to use as a loop cursor.
 * @tmp:		another 'type *' to use as temporary storage
 * @root:	'rb_root *' of the rbtree.
 * @field:	the name of the yx_rbnode field within 'type'.
 */
#define yx_core_rbnode_postorder_forEachSafe(cursor, n, root, field) \
    for (cursor = rb_entry_safe(yx_core_rbnode_postorder_firstNode(root), typeof(*cursor), field); \
        cursor && ({ n = rb_entry_safe(yx_core_rbnode_postorder_nextNode(&cursor->field), \
        typeof(*cursor), field); 1; }); \
        cursor = n)

#define yx_core_rbnode_ascending_forEachSafe(cursor, n, root, field) \

#endif /* defined(__YX_CORE_RBNODE_H_) */





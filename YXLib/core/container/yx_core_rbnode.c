//
//  yx_core_rbnode.c
//  YXLib
//
//  Created by LiuYuxi on 15/6/25.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_rbnode.h"


static struct yx_core_rbnode * _yx_core_rbnode_leftDeepestNode(const struct yx_core_rbnode *node);

////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma mark inline

yx_forceinline struct yx_core_rbnode *
__rb_red_parent(struct yx_core_rbnode *red)
{
    return (struct yx_core_rbnode *)red->__rb_parent_color;
}



yx_forceinline void
__rb_change_child(struct yx_core_rbnode *old, struct yx_core_rbnode *new,
                  struct yx_core_rbnode *parent, struct yx_core_rbroot *root)
{
    if (parent) {
        if (parent->rb_left == old)
            parent->rb_left = new;
        else
            parent->rb_right = new;
    } else
        root->yx_rbnode = new;
}

/*
 * Helper function for rotations:
 * - old's parent and color get assigned to new
 * - old gets assigned new as a parent and 'color' as a color.
 */

/*
 1.新节点代替旧节点原来的位置
 2.旧节点的父节点指向新节点
 
 @.新节点的字节点并没有指向父节点
 */

yx_forceinline void
__rb_rotate_set_parents(struct yx_core_rbnode *old, struct yx_core_rbnode *new,
                        struct yx_core_rbroot *root, int color)
{
    struct yx_core_rbnode *parent = yx_core_rbnode_parentNode(old);
    new->__rb_parent_color = old->__rb_parent_color;
    yx_core_rbnode_setParentAndColor(old, new, color);
    __rb_change_child(old, new, parent, root);
}

yx_forceinline struct yx_core_rbnode *
__rb_erase_augmented(struct yx_core_rbnode *node, struct yx_core_rbroot *root)
{
    struct yx_core_rbnode *child = node->rb_right, *tmp = node->rb_left;
    struct yx_core_rbnode *parent, *rebalance;
    yx_core_rbnode_magicpt magicpt;
    
    if (!tmp) {
        /*
         * Case 1: node to erase has no more than 1 child (easy!)
         *
         * Note that if there is one child it must be red due to 5)
         * and node must be black due to 4). We adjust colors locally
         * so as to bypass __rb_erase_color() later on.
         */
        magicpt = node->__rb_parent_color;
        parent = yx_core_rbnode_magicpt_parent(magicpt);
        __rb_change_child(node, child, parent, root);
        if (child) {
            child->__rb_parent_color = magicpt;
            rebalance = NULL;
        } else
            rebalance = yx_core_rbnode_magicpt_isBlack(magicpt) ? parent : NULL;
        tmp = parent;
    } else if (!child) {
        /* Still case 1, but this time the child is node->rb_left */
        tmp->__rb_parent_color = magicpt = node->__rb_parent_color;
        parent = yx_core_rbnode_magicpt_parent(magicpt);
        __rb_change_child(node, tmp, parent, root);
        rebalance = NULL;
        tmp = parent;
    } else {
        struct yx_core_rbnode *successor = child, *child2;
        tmp = child->rb_left;
        if (!tmp) {
            /*
             * Case 2: node's successor is its right child
             *
             *    (n)          (s)
             *    / \          / \
             *  (x) (s)  ->  (x) (c)
             *        \
             *        (c)
             */
            parent = successor;
            child2 = successor->rb_right;
        } else {
            /*
             * Case 3: node's successor is leftmost under
             * node's right child subtree
             *
             *    (n)          (s)
             *    / \          / \
             *  (x) (y)  ->  (x) (y)
             *      /            /
             *    (p)          (p)
             *    /            /
             *  (s)          (c)
             *    \
             *    (c)
             */
            do {
                parent = successor;
                successor = tmp;
                tmp = tmp->rb_left;
            } while (tmp);
            parent->rb_left = child2 = successor->rb_right;
            successor->rb_right = child;
            yx_core_rbnode_setParent(child, successor);
        }
        
        successor->rb_left = tmp = node->rb_left;
        yx_core_rbnode_setParent(tmp, successor);
        
        magicpt = node->__rb_parent_color;
        tmp = yx_core_rbnode_magicpt_parent(magicpt);
        __rb_change_child(node, successor, tmp, root);
        if (child2) {
            successor->__rb_parent_color = magicpt;
            yx_core_rbnode_setParentAndColor(child2, parent, YX_CORE_RBNODE_BLACK);
            rebalance = NULL;
        } else {
            yx_core_rbnode_magicpt magicpt_2 = successor->__rb_parent_color;
            successor->__rb_parent_color = magicpt;
            rebalance = yx_core_rbnode_magicpt_isBlack(magicpt_2) ? parent : NULL;
        }
        tmp = successor;
    }
    
    return rebalance;
}

/*
 * Inline version for yx_core_rbnode_erase() use - we want to be able to inline
 * and eliminate the dummy_rotate callback there
 */
yx_forceinline void
__yx_core_rbnode_banlanceEraseColor(struct yx_core_rbnode* parent, struct yx_core_rbroot* root)
{
    struct yx_core_rbnode *node = NULL, *sibling, *tmp1, *tmp2;
    
    while (yx_true) {
        /*
         * Loop invariants:
         * - node is black (or NULL on first iteration)
         * - node is not the root (parent is not NULL)
         * - All leaf paths going through parent and node have a
         *   black node count that is 1 lower than other leaf paths.
         */
        sibling = parent->rb_right;
        if (node != sibling) {	/* node == parent->rb_left */
            if (yx_core_rbnode_isRed(sibling)) {
                /*
                 * Case 1 - left rotate at parent
                 *
                 *     P               S
                 *    / \             / \
                 *   N   s    -->    p   Sr
                 *      / \         / \
                 *     Sl  Sr      N   Sl
                 */
                parent->rb_right = tmp1 = sibling->rb_left;
                sibling->rb_left = parent;
                yx_core_rbnode_setParentAndColor(tmp1, parent, YX_CORE_RBNODE_BLACK);
                __rb_rotate_set_parents(parent, sibling, root,
                                        YX_CORE_RBNODE_RED);
                sibling = tmp1;
            }
            tmp1 = sibling->rb_right;
            if (!tmp1 || yx_core_rbnode_isBlack(tmp1)) {
                tmp2 = sibling->rb_left;
                if (!tmp2 || yx_core_rbnode_isBlack(tmp2)) {
                    /*
                     * Case 2 - sibling color flip
                     * (p could be either color here)
                     *
                     *    (p)           (p)
                     *    / \           / \
                     *   N   S    -->  N   s
                     *      / \           / \
                     *     Sl  Sr        Sl  Sr
                     *
                     * This leaves us violating 5) which
                     * can be fixed by flipping p to black
                     * if it was red, or by recursing at p.
                     * p is red when coming from Case 1.
                     */
                    yx_core_rbnode_setParentAndColor(sibling, parent,
                                        YX_CORE_RBNODE_RED);
                    if (yx_core_rbnode_isRed(parent))
                        yx_core_rbnode_setBlack(parent);
                    else {
                        node = parent;
                        parent = yx_core_rbnode_parentNode(node);
                        if (parent)
                            continue;
                    }
                    break;
                }
                /*
                 * Case 3 - right rotate at sibling
                 * (p could be either color here)
                 *
                 *   (p)           (p)
                 *   / \           / \
                 *  N   S    -->  N   Sl
                 *     / \             \
                 *    sl  Sr            s
                 *                       \
                 *                        Sr
                 */
                sibling->rb_left = tmp1 = tmp2->rb_right;
                tmp2->rb_right = sibling;
                parent->rb_right = tmp2;
                if (tmp1)
                    yx_core_rbnode_setParentAndColor(tmp1, sibling,
                                        YX_CORE_RBNODE_BLACK);
                tmp1 = sibling;
                sibling = tmp2;
            }
            /*
             * Case 4 - left rotate at parent + color flips
             * (p and sl could be either color here.
             *  After rotation, p becomes black, s acquires
             *  p's color, and sl keeps its color)
             *
             *      (p)             (s)
             *      / \             / \
             *     N   S     -->   P   Sr
             *        / \         / \
             *      (sl) sr      N  (sl)
             */
            parent->rb_right = tmp2 = sibling->rb_left;
            sibling->rb_left = parent;
            yx_core_rbnode_setParentAndColor(tmp1, sibling, YX_CORE_RBNODE_BLACK);
            if (tmp2)
                yx_core_rbnode_setParent(tmp2, parent);
            __rb_rotate_set_parents(parent, sibling, root,
                                    YX_CORE_RBNODE_BLACK);
            break;
        } else {
            sibling = parent->rb_left;
            if (yx_core_rbnode_isRed(sibling)) {
                /* Case 1 - right rotate at parent */
                parent->rb_left = tmp1 = sibling->rb_right;
                sibling->rb_right = parent;
                yx_core_rbnode_setParentAndColor(tmp1, parent, YX_CORE_RBNODE_BLACK);
                __rb_rotate_set_parents(parent, sibling, root,
                                        YX_CORE_RBNODE_RED);
                sibling = tmp1;
            }
            tmp1 = sibling->rb_left;
            if (!tmp1 || yx_core_rbnode_isBlack(tmp1)) {
                tmp2 = sibling->rb_right;
                if (!tmp2 || yx_core_rbnode_isBlack(tmp2)) {
                    /* Case 2 - sibling color flip */
                    yx_core_rbnode_setParentAndColor(sibling, parent,
                                        YX_CORE_RBNODE_RED);
                    if (yx_core_rbnode_isRed(parent))
                        yx_core_rbnode_setBlack(parent);
                    else {
                        node = parent;
                        parent = yx_core_rbnode_parentNode(node);
                        if (parent)
                            continue;
                    }
                    break;
                }
                /* Case 3 - right rotate at sibling */
                sibling->rb_right = tmp1 = tmp2->rb_left;
                tmp2->rb_left = sibling;
                parent->rb_left = tmp2;
                if (tmp1)
                    yx_core_rbnode_setParentAndColor(tmp1, sibling,
                                        YX_CORE_RBNODE_BLACK);
                tmp1 = sibling;
                sibling = tmp2;
            }
            /* Case 4 - left rotate at parent + color flips */
            parent->rb_left = tmp2 = sibling->rb_right;
            sibling->rb_right = parent;
            yx_core_rbnode_setParentAndColor(tmp1, sibling, YX_CORE_RBNODE_BLACK);
            if (tmp2)
                yx_core_rbnode_setParent(tmp2, parent);
            __rb_rotate_set_parents(parent, sibling, root,
                                    YX_CORE_RBNODE_BLACK);
            break;
        }
    }
}


yx_forceinline void
__yx_core_rbnode_banlanceInsertColor(struct yx_core_rbnode* node, struct yx_core_rbroot* root)
{
    struct yx_core_rbnode* parent = __rb_red_parent(node), *gparent, *tmp;
    
    while (yx_true) {
        /*
         * Loop invariant: node is red
         *
         * If there is a black parent, we are done.
         * Otherwise, take some corrective action as we don't
         * want a red root or two consecutive red nodes.
         */
        if (!parent) {
            yx_core_rbnode_setParentAndColor(node, NULL, YX_CORE_RBNODE_BLACK);
            break;
        } else if (yx_core_rbnode_isBlack(parent))
            break;
        
        gparent = __rb_red_parent(parent);
        
        tmp = gparent->rb_right;
        if (parent != tmp) {	/* parent == gparent->rb_left */
            if (tmp && yx_core_rbnode_isRed(tmp)) {
                /*
                 * Case 1 - color flips
                 *
                 *       G            g
                 *      / \          / \
                 *     p   u  -->   P   U
                 *    /            /
                 *   n            n
                 *
                 * However, since g's parent might be red, and
                 * 4) does not allow this, we need to recurse
                 * at g.
                 */
                yx_core_rbnode_setParentAndColor(tmp, gparent, YX_CORE_RBNODE_BLACK);
                yx_core_rbnode_setParentAndColor(parent, gparent, YX_CORE_RBNODE_BLACK);
                node = gparent;
                parent = yx_core_rbnode_parentNode(node);
                yx_core_rbnode_setParentAndColor(node, parent, YX_CORE_RBNODE_RED);
                continue;
            }
            
            tmp = parent->rb_right;
            if (node == tmp) {
                /*
                 * Case 2 - left rotate at parent
                 *
                 *      G             G
                 *     / \           / \
                 *    p   U  -->    n   U
                 *     \           /
                 *      n         p
                 *
                 * This still leaves us in violation of 4), the
                 * continuation into Case 3 will fix that.
                 */
                parent->rb_right = tmp = node->rb_left;
                node->rb_left = parent;
                if (tmp)
                    yx_core_rbnode_setParentAndColor(tmp, parent,
                                                     YX_CORE_RBNODE_BLACK);
                yx_core_rbnode_setParentAndColor(parent, node, YX_CORE_RBNODE_RED);
                parent = node;
                tmp = node->rb_right;
            }
            
            /*
             * Case 3 - right rotate at gparent
             *
             *        G           P
             *       / \         / \
             *      p   U  -->  n   g
             *     /                 \
             *    n                   U
             */
            gparent->rb_left = tmp;  /* == parent->rb_right */
            parent->rb_right = gparent;
            if (tmp)
                yx_core_rbnode_setParentAndColor(tmp, gparent, YX_CORE_RBNODE_BLACK);
            __rb_rotate_set_parents(gparent, parent, root, YX_CORE_RBNODE_RED);
            break;
        } else {
            tmp = gparent->rb_left;
            if (tmp && yx_core_rbnode_isRed(tmp)) {
                /* Case 1 - color flips */
                yx_core_rbnode_setParentAndColor(tmp, gparent, YX_CORE_RBNODE_BLACK);
                yx_core_rbnode_setParentAndColor(parent, gparent, YX_CORE_RBNODE_BLACK);
                node = gparent;
                parent = yx_core_rbnode_parentNode(node);
                yx_core_rbnode_setParentAndColor(node, parent, YX_CORE_RBNODE_RED);
                continue;
            }
            
            tmp = parent->rb_left;
            if (node == tmp) {
                /* Case 2 - right rotate at parent */
                parent->rb_left = tmp = node->rb_right;
                node->rb_right = parent;
                if (tmp)
                    yx_core_rbnode_setParentAndColor(tmp, parent,
                                                     YX_CORE_RBNODE_BLACK);
                yx_core_rbnode_setParentAndColor(parent, node, YX_CORE_RBNODE_RED);
                parent = node;
                tmp = node->rb_left;
            }
            
            /* Case 3 - left rotate at gparent */
            gparent->rb_right = tmp;  /* == parent->rb_left */
            parent->rb_left = gparent;
            if (tmp)
                yx_core_rbnode_setParentAndColor(tmp, gparent, YX_CORE_RBNODE_BLACK);
            __rb_rotate_set_parents(gparent, parent, root, YX_CORE_RBNODE_RED);
            break;
        }
    }
    
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void yx_core_rbnode_insert(struct yx_core_rbnode* node, struct yx_core_rbnode* parent, struct yx_core_rbroot* root, struct yx_core_rbnode** rb_link)
{
    node->__rb_parent_color = (yx_core_rbnode_magicpt)parent;
    node->rb_left = node->rb_right = NULL;
    *rb_link = node;
    
    
    __yx_core_rbnode_banlanceInsertColor(node, root);
}




void  yx_core_rbnode_erase(struct yx_core_rbnode *node, struct yx_core_rbroot *root)
{
    struct yx_core_rbnode *rebalance;
    rebalance = __rb_erase_augmented(node, root);
    if (rebalance)
        __yx_core_rbnode_banlanceEraseColor(rebalance, root);
}


#pragma mark find logical
/* Find logical next and previous nodes in a tree */
struct yx_core_rbnode* yx_core_rbnode_nextNode(const struct yx_core_rbnode* node)
{
    struct yx_core_rbnode *parent;
    
    if (yx_core_rbnode_isWild(node))
        return NULL;
    
    /*
     * If we have a right-hand child, go down and then left as far
     * as we can.
     */
    if (node->rb_right) {
        node = node->rb_right;
        while (node->rb_left)
            node=node->rb_left;
        return (struct yx_core_rbnode *)node;
    }
    
    /*
     * No right-hand children. Everything down and left is smaller than us,
     * so any 'next' node must be in the general direction of our parent.
     * Go up the tree; any time the ancestor is a right-hand child of its
     * parent, keep going up. First time it's a left-hand child of its
     * parent, said parent is our 'next' node.
     */
    while ((parent = yx_core_rbnode_parentNode(node)) && node == parent->rb_right)
        node = parent;
    
    return parent;
}
struct yx_core_rbnode* yx_core_rbnode_prevNode(const struct yx_core_rbnode* node)
{
    struct yx_core_rbnode *parent;
    
    if (yx_core_rbnode_isWild(node))
        return NULL;
    
    /*
     * If we have a left-hand child, go down and then right as far
     * as we can.
     */
    if (node->rb_left) {
        node = node->rb_left;
        while (node->rb_right)
            node=node->rb_right;
        return (struct yx_core_rbnode *)node;
    }
    
    /*
     * No left-hand children. Go up till we find an ancestor which
     * is a right-hand child of its parent.
     */
    while ((parent = yx_core_rbnode_parentNode(node)) && node == parent->rb_left)
        node = parent;
    
    return parent;
}
struct yx_core_rbnode* yx_core_rbnode_firstNode(const struct yx_core_rbroot* root)
{
    struct yx_core_rbnode	*n;
    
    n = root->yx_rbnode;
    if (!n)
        return NULL;
    while (n->rb_left)
        n = n->rb_left;
    return n;
}

struct yx_core_rbnode* yx_core_rbnode_lastNode(const struct yx_core_rbroot* root)
{
    struct yx_core_rbnode	*n;
    
    n = root->yx_rbnode;
    if (!n)
        return NULL;
    while (n->rb_right)
        n = n->rb_right;
    return n;
}


/* Postorder iteration - always visit the parent after its children */
struct yx_core_rbnode* yx_core_rbnode_postorder_firstNode(const struct yx_core_rbroot* root)
{
    if (!root->yx_rbnode)
        return NULL;
    
    return _yx_core_rbnode_leftDeepestNode(root->yx_rbnode);
}
struct yx_core_rbnode* yx_core_rbnode_postorder_nextNode(const struct yx_core_rbnode* node)
{
    const struct yx_core_rbnode *parent;
    if (!node)
        return NULL;
    
    parent = yx_core_rbnode_parentNode(node);
    
    /* If we're sitting on node, we've already seen our children */
    if (parent && node == parent->rb_left && parent->rb_right) {
        /* If we are the parent's left node, go to the parent's right
         * node then all the way down to the left */
        return _yx_core_rbnode_leftDeepestNode(parent->rb_right);
    } else
    /* Otherwise we are the parent's right node, and the parent
     * should be next */
        return (struct yx_core_rbnode *)parent;
}




#pragma mark replace node
void rb_replace_node(struct yx_core_rbnode *victim, struct yx_core_rbnode *newone,
                            struct yx_core_rbroot *root)
{
    struct yx_core_rbnode *parent = yx_core_rbnode_parentNode(victim);
    
    /* Set the surrounding nodes to point to the replacement */
    __rb_change_child(victim, newone, parent, root);
    if (victim->rb_left)
        yx_core_rbnode_setParent(victim->rb_left, newone);
    if (victim->rb_right)
        yx_core_rbnode_setParent(victim->rb_right, newone);
    
    /* Copy the pointers/colour from the victim to the replacement */
    *newone = *victim;
}



#pragma mark private
/*
 返回node节点最左边的叶子节点
 */
static struct yx_core_rbnode * _yx_core_rbnode_leftDeepestNode(const struct yx_core_rbnode *node)
{
    for (;;) {
        if (node->rb_left)
            node = node->rb_left;
        else if (node->rb_right)
            node = node->rb_right;
        else
            return (struct yx_core_rbnode *)node;
    }
}



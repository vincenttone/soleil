#include <stdlib.h>
#include "sol_rbtree.h"

SolRBTree* solRBTree_new()
{
    SolRBTree *t = sol_calloc(1, sizeof(SolRBTree));
    if (t == NULL) {
        return NULL;
    }
    solRBTree_set_nil(t, sol_calloc(1, sizeof(SolRBTreeNode)));
    if (solRBTree_nil(t) == NULL) {
        solRBTree_free(t);
        return NULL;
    }
    solRBTreeNode_dye_black(solRBTree_nil(t));
    solRBTree_set_root(t, solRBTree_nil(t));
    return t;
}

int solRBTree_node_free(SolRBTree *t, SolRBTreeNode *n)
{
    if (n) {
        if (solRBTree_node_val_free_func(t)) {
            solRBTree_node_val_free(solRBTreeNode_val(n));
        }
        sol_free(n);
    }
    return 0;
}

int _solRBTree_node_free(SolRBTree *t, SolRBTreeNode *n, void *d)
{
    return solRBTree_node_free(t, n);
}

void solRBTree_free(SolRBTree *t)
{
    solRBTree_travelsal_backorder(t, solRBTree_root(t), &_solRBTree_node_free, NULL);
    sol_free(solRBTree_nil(t));
    sol_free(t);
}

/**
 * left rorate brtree
 *
 *    x               y
 *   / \             / \
 *  a   y           x   c
 *      / \        / \
 *     b   c      a   b
 *
 * @params v_br_tree *tree
 * @params v_br_node *node
 **/
int solRBTree_left_rorate(SolRBTree *tree, SolRBTreeNode *node)
{
    if (solRBTree_node_right_is_nil(tree, node)) {
        // without right child! can not rorate
        return 1;
    }
    // node which chanage to node position
    SolRBTreeNode *replace2_node = solRBTreeNode_right(node);
    // change right child's left child to node's right child
    solRBTreeNode_set_right(node, solRBTreeNode_left(replace2_node));
    if (solRBTree_node_is_NOT_nil(tree, solRBTreeNode_right(node))) {
        solRBTreeNode_set_right_parent(node, node);
    }
    // change new node and it's parent relation
    solRBTreeNode_set_parent(replace2_node, solRBTreeNode_parent(node));
    if (solRBTree_node_is_root(tree, node)) {
        solRBTree_set_root(tree, replace2_node);
    } else if (solRBTreeNode_is_left(node)) {
        solRBTreeNode_set_parent_left(node, replace2_node);
    } else { // node is right child
        solRBTreeNode_set_parent_right(node, replace2_node);
    }
    // change node and new node relation
    solRBTreeNode_set_left(replace2_node, node);
    solRBTreeNode_set_parent(node, replace2_node);
    return 0;
}
/**
 * right rorate tree
 *
 *     x           y
 *    / \         / \
 *   y   c       a   x
 *  / \             / \
 * a   b           b   c
 *
 * @params v_br_tree *tree
 * @params v_br_node *node
 **/
int solRBTree_right_rorate(SolRBTree *tree, SolRBTreeNode *node)
{
    if (solRBTree_node_left_is_nil(tree, node)) {
        // without left child! can not rorate
        return 1;
    }
    // the node which chanage to current node position
    SolRBTreeNode *replace2_node = solRBTreeNode_left(node);
    // change left child's right child to node's left child
    solRBTreeNode_set_left(node, solRBTreeNode_right(replace2_node));
    if (solRBTree_node_is_NOT_nil(tree, solRBTreeNode_left(node))) {
        solRBTreeNode_set_left_parent(node, node);
    }
    // change new node and it's parent relation
    solRBTreeNode_set_parent(replace2_node, solRBTreeNode_parent(node));
    if (solRBTree_node_is_root(tree, node)) {
        solRBTree_set_root(tree, replace2_node);
    } else if (solRBTreeNode_is_left(node)) {
        solRBTreeNode_set_parent_left(node, replace2_node);
    } else { // node is right child
        solRBTreeNode_set_parent_right(node, replace2_node);
    }
    // change node and new node relation
    solRBTreeNode_set_right(replace2_node, node);
    solRBTreeNode_set_parent(node, replace2_node);
    return 0;
}

void solRBTree_insert_fixup(SolRBTree *tree, SolRBTreeNode *node)
{
    // only fixup when parent is red
    while (solRBTreeNode_parent_is_red(node)) {
        int is_left_branch = 1; // parent is left child 0, right child 1
        SolRBTreeNode *uncle;
        if (solRBTreeNode_parent_is_left(node)) {
            // parent is left child
            is_left_branch = 1;
            uncle = solRBTreeNode_right_elder(node);
        } else {
            is_left_branch = 0;
            uncle = solRBTreeNode_left_elder(node);
        }
        // cond 1: uncle is red
        if (solRBTreeNode_is_red(uncle)) {
            // change parent and uncle color to black
            solRBTreeNode_dye_black(uncle);
            solRBTreeNode_dye_black(solRBTreeNode_parent(node));
            // change parent's parent color to red
            solRBTreeNode_dye_red(solRBTreeNode_parent(uncle));
            // now insert node is parent's parent
            node = solRBTreeNode_parent(uncle);
        } else {
            if (is_left_branch) {
                // cond 2: left branch, right child
                if (solRBTreeNode_is_right(node)) {
                    node = solRBTreeNode_parent(node);
                    solRBTree_left_rorate(tree, node);
                }
                // cond3: left branch, left child
                solRBTreeNode_dye_black(solRBTreeNode_parent(node));
                solRBTreeNode_dye_red(solRBTreeNode_grandad(node));
                solRBTree_right_rorate(tree, solRBTreeNode_grandad(node));
            } else {
                if (solRBTreeNode_is_left(node)) {
                    node = solRBTreeNode_parent(node);
                    solRBTree_right_rorate(tree, node);
                }
                solRBTreeNode_dye_black(solRBTreeNode_parent(node));
                solRBTreeNode_dye_red(solRBTreeNode_grandad(node));
                solRBTree_left_rorate(tree, solRBTreeNode_grandad(node));
            }
        }
    }
    solRBTreeNode_dye_black(solRBTree_root(tree));
}

int solRBTree_insert(SolRBTree *tree, void *val)
{
    SolRBTreeNode *node;
    node = sol_alloc(sizeof(SolRBTreeNode));
    if (node == NULL) {
        return -1;
    }
    solRBTreeNode_dye_red(node);
    solRBTreeNode_set_parent(node, solRBTree_nil(tree));
    solRBTreeNode_set_left(node, solRBTree_nil(tree));
    solRBTreeNode_set_right(node, solRBTree_nil(tree));
    solRBTreeNode_set_val(node, val);
    // find insert position
    int w;
    SolRBTreeNode *current_node = solRBTree_root(tree);
    SolRBTreeNode *pre_node = solRBTree_nil(tree);
    while (solRBTree_node_is_NOT_nil(tree, current_node)) {
        pre_node = current_node;
        w = solRBTree_node_val_compare(tree, val, solRBTreeNode_val(current_node), 0x2);
        // w = solRBTree_node_compare(tree, node, current_node);
        if (w == 0) {
            // has this node
			if (solRBTree_insert_conflict_fix_func(tree)) {
				if (solRBTree_insert_conflict_fix(tree, val, solRBTreeNode_val(current_node)) !=0) {
					return 1;
				}
			}
            solRBTree_node_free(tree, node);
            return 0;
        } else if (w < 0) {
            current_node = solRBTreeNode_left(current_node);
        } else {
            current_node = solRBTreeNode_right(current_node);
        }
    }
    solRBTreeNode_set_parent(node, pre_node);
    if (solRBTree_node_is_nil(tree, pre_node)) {
        // empty tree
        solRBTree_set_root(tree, node);
    } else if (solRBTree_node_val_compare(tree, solRBTreeNode_val(node), solRBTreeNode_val(pre_node), 0x2) < 0) {
        // is left child
        solRBTreeNode_set_left(pre_node, node);
    } else {
        // is right child
        solRBTreeNode_set_right(pre_node, node);
    }
    solRBTree_insert_fixup(tree, node);
    solRBTree_count_inc(tree);
    return 0;
}
/**
 * search node
 * @params SolRBTree*
 * @params void*
 * @return void*
 * @return NULL
 */
void* solRBTree_search(SolRBTree *tree, void *val)
{
    SolRBTreeNode *n;
    n = solRBTree_search_node(tree, val);
    if (solRBTree_node_is_nil(tree, n)) {
        return NULL;
    }
    return solRBTreeNode_val(n);
}
/**
 * search the needle key node
 * @params v_br_tree *tree
 * @params long key
 * @return v_br_node || NIL
 *         success: v_br_node
 *         failed: NIL
 */
SolRBTreeNode* solRBTree_search_node(SolRBTree *tree, void *val)
{
    SolRBTreeNode *current_node = solRBTree_root(tree);
    int i = 0;
    while (solRBTree_node_is_NOT_nil(tree, current_node)) {
        i = solRBTree_node_val_compare(tree, val, solRBTreeNode_val(current_node), 0x1);
        if (i < 0) {
            current_node =  solRBTreeNode_left(current_node);
        } else if (i > 0) {
            current_node = solRBTreeNode_right(current_node);
        } else {
            break;
        }
    }
    return current_node;
}
/**
 * find min node in node's sub tree
 * @params tree
 * @params node
 * @return min node
 */
SolRBTreeNode* solRBTree_search_min_node(SolRBTree *tree, SolRBTreeNode *node)
{
    if (solRBTree_node_is_nil(tree, node)) {
        return solRBTree_nil(tree);
    }
    while (solRBTree_node_is_NOT_nil(tree, solRBTreeNode_left(node))) {
        node = solRBTreeNode_left(node);
    }
    return node;
}
/**
 * find max node int node's sub tree
 */
SolRBTreeNode* solRBTree_search_max_node(SolRBTree *tree, SolRBTreeNode *node)
{
    if (solRBTree_node_is_nil(tree, node)) {
        return solRBTree_nil(tree);
    }
    while (solRBTree_node_is_NOT_nil(tree, solRBTreeNode_right(node))) {
        node = solRBTreeNode_right(node);
    }
    return node;
}

void* solRBTree_min(SolRBTree *tree)
{
    SolRBTreeNode *node = solRBTree_search_min_node(tree, solRBTree_root(tree));
    if (solRBTree_node_is_nil(tree, node)) {
        return NULL;
    }
    return solRBTreeNode_val(node);
}

void* solRBTree_max(SolRBTree *tree)
{
    SolRBTreeNode *node = solRBTree_search_max_node(tree, solRBTree_root(tree));
    if (solRBTree_node_is_nil(tree, node)) {
        return NULL;
    }
    return solRBTreeNode_val(node);
}
void* solRBTree_max(SolRBTree*);
/**
 * find in order successor of the node in tree
 * when find in order successor:
 * print out the left node, then the right node
 * just think of this, it should be easy
 */
SolRBTreeNode* solRBTree_search_successor(SolRBTree *tree, SolRBTreeNode *node)
{
    if (solRBTree_node_is_nil(tree, node)) {
        return solRBTree_nil(tree);
    }
    // has right branch
    // find the min node in right branch
    if (solRBTree_node_is_NOT_nil(tree, solRBTreeNode_right(node))) {
        return solRBTree_search_min_node(tree, solRBTreeNode_right(node));
    }
    // right branch not exists
    // cond 1: node is left child, successor is parent;
    // cond 2: node is right child, find up until match cond 1.
    SolRBTreeNode *s_node = solRBTreeNode_parent(node);
    while (solRBTree_node_is_NOT_nil(tree, s_node) && node == solRBTreeNode_right(s_node)) {
        node = s_node;
        s_node = solRBTreeNode_parent(node);
    }
    return node;
}

/**
 * fixup colors and struct after delete
 * @param v_br_tree
 * @param v_br_node
 * @return void
 */
void solRBTree_delete_fixup(SolRBTree *tree, SolRBTreeNode *node)
{
    while (node != solRBTree_root(tree) && solRBTreeNode_is_black(node)) {
        int is_left_branch = solRBTreeNode_is_left(node) ? 1 : 0;
        SolRBTreeNode *brother = is_left_branch
            ? solRBTreeNode_right(solRBTreeNode_parent(node))
            : solRBTreeNode_left(solRBTreeNode_parent(node));
        if (solRBTreeNode_is_red(brother)) {
            // when brother is red, convert to black-brother
            solRBTreeNode_dye_black(brother);
            solRBTreeNode_dye_red(solRBTreeNode_parent(node));
            if (is_left_branch) {
                solRBTree_left_rorate(tree, solRBTreeNode_parent(node));
                brother = solRBTreeNode_right(solRBTreeNode_parent(node));
            } else {
                solRBTree_right_rorate(tree, solRBTreeNode_parent(node));
                brother = solRBTreeNode_left(solRBTreeNode_parent(node));
            }
        }
        // when black-brother
        if (solRBTreeNode_is_black(solRBTreeNode_left(brother))
            && solRBTreeNode_is_black(solRBTreeNode_right(brother))
            ) {
            // when brother has double-black-children
            // convert to delete_fixup parent node
            solRBTreeNode_dye_red(brother);
            node = solRBTreeNode_parent(node);
        } else {
            if (is_left_branch) {
                if (solRBTreeNode_is_black(solRBTreeNode_right(brother))) {
                    // when brother has black-right-red-left-children
                    // convert to brother has right-red-child
                    solRBTreeNode_dye_black(solRBTreeNode_left(brother));
                    solRBTreeNode_dye_red(brother);
                    solRBTree_right_rorate(tree, brother);
                    brother = solRBTreeNode_right(solRBTreeNode_parent(node));
                }
            } else {
                if (solRBTreeNode_is_black(solRBTreeNode_left(brother))) {
                    solRBTreeNode_dye_black(solRBTreeNode_right(brother));
                    solRBTreeNode_dye_red(brother);
                    solRBTree_left_rorate(tree, brother);
                    brother = solRBTreeNode_left(solRBTreeNode_parent(node));
                }
            }
            // when brother has right-red-child (in left branch)
            solRBTreeNode_dye(brother, solRBTreeNode_color(solRBTreeNode_parent(node)));
            solRBTreeNode_dye_black(solRBTreeNode_parent(node));
            if (is_left_branch) {
                solRBTreeNode_dye_black(solRBTreeNode_right(brother));
                solRBTree_left_rorate(tree, solRBTreeNode_parent(node));
            } else {
                solRBTreeNode_dye_black(solRBTreeNode_left(brother));
                solRBTree_right_rorate(tree, solRBTreeNode_parent(node));
            }
            node = solRBTree_root(tree);
        }
    }
    // red-black node to black
    solRBTreeNode_dye_black(node);
}

/**
 * delete node match the key int tree
 */
int solRBTree_del(SolRBTree *tree, void *val)
{
    SolRBTreeNode *del_node = solRBTree_root(tree);
    int i = 0;
    while (solRBTree_node_is_NOT_nil(tree, del_node)) {
        i = solRBTree_node_val_compare(tree, val, solRBTreeNode_val(del_node), 0x4);
        if (i < 0) {
            del_node =  solRBTreeNode_left(del_node);
        } else if (i > 0) {
            del_node = solRBTreeNode_right(del_node);
        } else {
            break;
        }
    }
    return  solRBTree_delete_node(tree, del_node);
}

int solRBTree_delete_node(SolRBTree *tree, SolRBTreeNode *del_node)
{
    if (solRBTree_node_is_nil(tree, del_node)) {
        return 1;
    }
    // find the real position to del
    // cond 1: only one child branch, del the node
    // cond 2: two child branch, find the successor
    SolRBTreeNode *rp_node;
    if (solRBTree_node_is_nil(tree, solRBTreeNode_left(del_node))
        || solRBTree_node_is_nil(tree, solRBTreeNode_right(del_node))
    ) {
        rp_node = del_node;
    } else {
        rp_node = solRBTree_search_successor(tree, del_node);
    }
    SolRBTreeNode *rp_child_node = solRBTree_node_is_nil(tree, solRBTreeNode_left(rp_node))
        ? solRBTreeNode_right(rp_node)
        : solRBTreeNode_left(rp_node);
    // adjust relations
    solRBTreeNode_set_parent(rp_child_node, solRBTreeNode_parent(rp_node));
    if (solRBTree_node_is_root(tree, rp_node)) {
        // rp_node is root
        solRBTree_set_root(tree, rp_child_node);
    } else if (solRBTreeNode_is_left(rp_node)) {
        // rp_node is left child
        solRBTreeNode_set_left(solRBTreeNode_parent(rp_node), rp_child_node);
    } else {
        // rp_node is right child
        solRBTreeNode_set_right(solRBTreeNode_parent(rp_node), rp_child_node);
    }
    // fix the key
    if (del_node != rp_node) {
        void *pre_val = solRBTreeNode_val(del_node);
        solRBTreeNode_set_val(del_node, solRBTreeNode_val(rp_node));
        solRBTreeNode_set_val(rp_node, pre_val);
    }
    // if deleted node is black, need to fixup
    if (solRBTreeNode_is_black(rp_node)) {
        solRBTree_delete_fixup(tree, rp_child_node);
    }
    // delete the node
    solRBTree_node_free(tree, rp_node);
    solRBTree_count_dec(tree);
    return 0;
}

int solRBTree_travelsal_inorder(SolRBTree *tree, SolRBTreeNode *node, solRBTree_f_ptr_act f, void *d)
{
    if (solRBTree_node_is_nil(tree, node)) return 0;
    solRBTree_travelsal_inorder(tree, solRBTreeNode_left(node), f, d);
    int r = (*f)(tree, node, d);
    if (r != 0) return r;
    solRBTree_travelsal_inorder(tree, solRBTreeNode_right(node), f, d);
    return 0;
}

int solRBTree_travelsal_preorder(SolRBTree *tree, SolRBTreeNode *node, solRBTree_f_ptr_act f, void *d)
{
    if (solRBTree_node_is_nil(tree, node)) return 0;
    int r = (*f)(tree, node, d);
    if (r != 0) return r;
    solRBTree_travelsal_preorder(tree, solRBTreeNode_left(node), f, d);
    solRBTree_travelsal_preorder(tree, solRBTreeNode_right(node), f, d);
    return 0;
}


int solRBTree_travelsal_backorder(SolRBTree *tree, SolRBTreeNode *node, solRBTree_f_ptr_act f, void *d)
{
    if (solRBTree_node_is_nil(tree, node)) return 0;
    solRBTree_travelsal_backorder(tree, solRBTreeNode_left(node), f, d);
    solRBTree_travelsal_backorder(tree, solRBTreeNode_right(node), f, d);
    int r = (*f)(tree, node, d);
    if (r != 0) return r;
    return 0;
}

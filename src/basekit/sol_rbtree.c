#include <stdlib.h>
#include "brtree.h"

SolRbt* solRbt_new()
{
    SolRbt *t = sol_calloc(1, sizeof(SolRbt));
    if (t == NULL) {
        return NULL;
    }
    solRbt_set_nil(t, sol_calloc(1, sizeof(SolRbtNode)));
    if (solRbt_nil(t) == NULL) {
        solRbt_free(t);
        return NULL;
    }
    solRbtNode_dye_black(solRbt_nil(t));
    solRbt_set_root(solRbt_nil(t));
    return t;
}

void solRbt_node_free(SolRbt *t, SolRbtNode *n)
{
    if (n) {
        if (solRbt_node_val_free_func(t)) {
            solRbt_node_val_free(solRbtNode_val(n));
        }
        sol_free(n);
    }
}

void solRbt_free(SolRbt *t)
{
    solRbt_travelsal_backorder(t, solRbtNode_root(t), &solRbt_node_free);
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
static int solRbt_left_rorate(SolRbt *tree, SolRbtNode *node)
{
    if (solRbt_node_right_is_nil(tree, node)) {
        // without right child! can not rorate
        return 1;
    }
    // node which chanage to node position
    SolRbtNode *replace2_node = solRbtNode_right(node);
    // change right child's left child to node's right child
    solRbtNode_set_right(node, solRbtNode_left(replace2_node));
    if (solRbt_node_is_NOT_nil(tree, solRbtNode_right(node))) {
        solRbtNode_set_right_parent(node, node);
    }
    // change new node and it's parent relation
    solRbtNode_set_parent(replace2_node, solRbtNode_parent(node));
    if (solRbt_node_is_root(tree, node)) {
        solRbt_set_root(tree, replace2_node);
    } else if (solRbtNode_is_left(node)) {
        solRbtNode_set_parent_left(node, replace2_node);
    } else { // node is right child
        solRbtNode_set_parent_right(node, replace2_node);
    }
    // change node and new node relation
    solRbtNode_set_left(replace2_node, node);
    solRbtNode_set_parent(node, replace2_node);
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
static int solRbt_right_rorate(SolRbt *tree, SolRbtNode *node)
{
    if (solRbt_node_left_is_nil(tree, node)) {
        // without left child! can not rorate
        return 1;
    }
    // the node which chanage to current node position
    SolRbtNode *replace2_node = solRbtNode_left(node);
    // change left child's right child to node's left child
    solRbtNode_set_left(node, solRbtNode_right(replace2_node));
    if (solRbt_node_is_NOT_nil(tree, solRbtNode_left(node))) {
        solRbtNode_set_left_parent(node, node);
    }
    // change new node and it's parent relation
    solRbtNode_set_parent(replace2_node, solRbtNode_parent(node));
    if (solRbt_node_is_root(node)) {
        solRbt_set_root(replace2_node);
    } else if (solRbtNode_is_left(node)) {
        solRbtNode_set_parent_left(node, replace2_node);
    } else { // node is right child
        solRbtNode_set_parent_right(node, replace2_node);
    }
    // change node and new node relation
    solRbtNode_set_right(replace2_node, node);
    solRbtNode_set_parent(node, replace2_node);
    return 0;
}

static void solRbt_insert_fixup(SolRbt *tree, SolRbtNode *node)
{
    // only fixup when parent is red
    while (solRbtNode_parent_is_red(node)) {
        int is_left_branch = 1; // parent is left child 0, right child 1
        SolRbtNode *uncle;
        if (solRbtNode_parent_is_left(node)) {
            // parent is left child
            is_left_branch = 1;
            uncle = solRbtNode_right_elder(node);
        } else {
            is_left_branch = 0;
            uncle = solRbtNode_left_elder(node);
        }
        // cond 1: uncle is red
        if (solRbtNode_is_red(uncle)) {
            // change parent and uncle color to black
            solRbtNode_dye_black(uncle);
            solRbtNode_dye_black(solRbtNode_parent(node));
            // change parent's parent color to red
            solRbtNode_dye_red(solRbtNode_parent(uncle));
            // now insert node is parent's parent
            node = solRbtNode_parent(uncle);
        } else {
            if (is_left_branch) {
                // cond 2: left branch, right child
                if (solRbtNode_is_right(node)) {
                    node = solRbtNode_parent(node);
                    solRbt_left_rorate(tree, node);
                }
                // cond3: left branch, left child
                solRbtNode_dye_black(solRbtNode_parent(node));
                solRbtNode_dye_red(solRbtNode_grandad(node));
                solRbt_right_rorate(tree, solRbtNode_grandad(node));
            } else {
                if (solRbtNode_is_left(node)) {
                    node = solRbtNode_parent(node);
                    solRbt_right_rorate(tree, node);
                }
                solRbtNode_dye_black(solRbtNode_parent(node));
                solRbtNode_dye_red(solRbtNode_grandad(node));
                solRbt_left_rorate(tree, solRbtNode_grandad(node));
            }
        }
    }
    solRbtNode_dye_black(solRbt_root(tree));
}

SolRbtNode* solRbt_insert(SolRbt *tree, void *val)
{
    SolRbtNode *node;
    node = sol_alloc(sizeof(SolRbtNode));
    if (node == NULL) {
        return NULL;
    }
    solRbtNode_dye_red(node);
    solRbtNode_set_weight(node, solRbt_node_val_hash(val));
    solRbtNode_set_parent(node, solRbt_nil(tree));
    solRbtNode_set_left(node, solRbt_nil(tree));
    solRbtNode_set_right(node, solRbt_nil(tree));
    // find insert position
    SolRbtNode *current_node = solRbt_root(tree);
    SolRbtNode *pre_node = solRbt_nil(tree);
    while (solRbt_node_is_NOT_nil(current_node)) {
        pre_node = current_node;
        if (solRbtNode_weight(current_node) == solRbtNode_weight(node)) {
            // has this node
            return;
        } else if (solRbtNode_weight(current_node) > solRbtNode_weight(node)) {
            current_node = solRbtNode_left(current_node);
        } else {
            current_node = solRbtNode_right(current_node);
        }
    }
    solRbtNode_set_parent(node, pre_node);
    if (solRbt_node_is_nil(tree, pre_node)) {
        // empty tree
        solRbt_set_root(tree, node);
    } else if (solRbtNode_weight(node) < solRbtNode_weight(pre_node)) {
        // is left child
        solRbtNode_set_left(pre_node, node);
    } else {
        // is right child
        solRbtNode_set_right(pre_node, node);
    }
    solRbt_insert_fixup(tree, node);
}

/**
 * search the needle key node
 * @params v_br_tree *tree
 * @params long key
 * @return v_br_node || NIL
 *         success: v_br_node
 *         failed: NIL
 */
SolRbtNode* solRbt_search_node(SolRbt *tree, void *val)
{
    size_t weight = solRbt_node_val_hash(val);
    SolRbtNode *current_node = solRbt_root(tree);
    while (solRbt_node_is_NOT_nil(tree, current_node)
           && weight != solRbtNode_weight(current_node)
        ) {
        current_node = weight < solRbtNode_weight(current_node)
            ? solRbtNode_left(current_node)
            : solRbtNode_right(current_node);
    }
    return current_node;
}
/**
 * find min node in node's sub tree
 * @params tree
 * @params node
 * @return min node
 */
SolRbtNode* solRbt_search_min_node(SolRbt *tree, SolRbtNode *node)
{
    if (solRbt_node_is_nil(tree, node)) {
        return solRbt_nil(tree);
    }
    while (solRbt_node_is_NOT_nil(tree, solRbtNode_left(node))) {
        node = solRbtNode_left(node);
    }
    return node;
}
/**
 * find max node int node's sub tree
 */
SolRbtNode* solRbt_search_max_node(SolRbt *tree, SolRbtNode *node)
{
    if (solRbt_node_is_nil(tree, node)) {
        return solRbt_nil(tree);
    }
    while (solRbt_node_is_NOT_nil(tree, solRbtNode_right(node))) {
        node = solRbtNode_right(node);
    }
    return node;
}
/**
 * find in order successor of the node in tree
 * when find in order successor:
 * print out the left node, then the right node
 * just think of this, it should be easy
 */
SolRbtNode* solRbt_search_successor(SolRbt *tree, SolRbtNode *node)
{
    if (solRbt_node_is_nil(tree, node)) {
        return solRbt_nil(tree);
    }
    // has right branch
    // find the min node in right branch
    if (solRbt_node_is_NOT_nil(solRbtNode_right(node))) {
        return solRbt_search_min_node(tree, solRbtNode_right(node));
    }
    // right branch not exists
    // cond 1: node is left child, successor is parent;
    // cond 2: node is right child, find up until match cond 1.
    SolRbtNode *s_node = solRbtNode_parent(node);
    while (solRbt_node_is_NOT_nil(tree, s_node) && node == solRbtNode_right(s_node)) {
        node = s_node;
        s_node = solRbtNode_parent(node);
    }
    return node;
}

/**
 * fixup colors and struct after delete
 * @param v_br_tree
 * @param v_br_node
 * @return void
 */
static void solRbt_delete_fixup(SolRbt *tree, SolRbtNode *node)
{
    while (node != solRbt_root(tree) && solRbtNode_is_black(node)) {
        int is_left_branch = solRbtNode_is_left(node) ? 1 : 0;
        SolRbtNode *brother = is_left_branch
            ? solRbtNode_right(solRbtNode_parent(node))
            : solRbtNode_left(solRbtNode_parent(node));
        if (solRbtNode_is_red(brother)) {
            // when brother is red, convert to black-brother
            solRbtNode_dye_black(brother);
            solRbtNode_dye_red(solRbtNode_parent(node));
            if (is_left_branch) {
                solRbt_left_rorate(tree, solRbtNode_parent(node));
                brother = solRbtNode_right(solRbtNode_parent(node));
            } else {
                solRbt_right_rorate(tree, solRbtNode_parent(node));
                brother = solRbtNode_left(solRbtNode_parent(node));
            }
        }
        // when black-brother
        if (solRbtNode_is_black(solRbtNode_left(brother))
            && solRbtNode_is_black(solRbtNode_right(brother))
            ) {
            // when brother has double-black-children
            // convert to delete_fixup parent node
            solRbtNode_dye_red(brother);
            node = solRbtNode_parent(node);
        } else {
            if (is_left_branch) {
                if (solRbtNode_is_black(solRbtNode_right(brother))) {
                    // when brother has black-right-red-left-children
                    // convert to brother has right-red-child
                    solRbtNode_dye_black(solRbtNode_left(brother));
                    solRbtNode_dye_red(brother);
                    solRbt_right_rorate(tree, brother);
                    brother = solRbtNode_right(solRbtNode_parent(node));
                }
            } else {
                if (solRbtNode_is_black(solRbtNode_left(brother))) {
                    solRbtNode_dye_black(solRbtNode_right(brother));
                    solRbtNode_dye_red(brother);
                    solRbt_left_rorate(tree, brother);
                    brother = solRbtNode_left(solRbtNode_parent(node));
                }
            }
            // when brother has right-red-child (in left branch)
            solRbtNode_dye(brother, solRbtNode_color(solRbtNode_parent(node)));
            solRbtNode_dye_black(solRbtNode_parent(node));
            if (is_left_branch) {
                solRbtNode_dye_black(solRbtNode_right(brother));
                solRbt_left_rorate(tree, solRbtNode_parent(node));
            } else {
                solRbtNode_dye_black(solRbtNode_left(brother));
                solRbt_right_rorate(tree, solRbtNode_parent(node));
            }
            node = solRbt_root(tree);
        }
    }
    // red-black node to black
    solRbtNode_dye_black(node);
}

/**
 * delete node match the key int tree
 */
int solRbt_del(SolRbt *tree, void *val)
{
    SolRbtNode *del_node = solRbt_search_node(tree, val);
    if (solRbt_node_is_nil(tree, del_node)) {
        return 1;
    }
    // find the real position to del
    // cond 1: only one child branch, del the node
    // cond 2: two child branch, find the successor
    SolRbtNode *rp_node = (solRbt_node_is_nil(tree, solRbtNode_left(del_node))
                           || solRbt_node_is_nil(tree, solRbtNode_right(del_node)))
        ? del_node
        : solRbt_search_successor(tree, del_node);
    SolRbtNode *rp_child_node = solRbt_node_is_nil(tree, solRbtNode_left(rp_node))
        ? solRbtNode_right(rp_node)
        : solRbtNode_left(rp_node);
    // adjust relations
    solRbtNode_set_parent(rp_child_node, solRbtNode_parent(rp_node));
    if (solRbt_node_is_root(tree, rp_node)) {
        // rp_node is root
        solRbt_set_root(tree, rp_child_node);
    } else if (solRbtNode_is_left(rp_node)) {
        // rp_node is left child
        solRbtNode_set_left(solRbtNode_parent(rp_node), rp_child_node);
    } else {
        // rp_node is right child
        solRbtNode_set_right(solRbtNode_parent(rp_node), rp_child_node);
    }
    // fix the key
    if (del_node != rp_node) {
        solRbtNode_set_weight(del_node, solRbtNode_weight(rp_node));
        solRbtNode_set_val(del_node, solRbtNode_val(rp_node));
    }
    // if deleted node is black, need to fixup
    if (solRbtNode_is_black(rp_node)) {
        solRbt_delete_fixup(tree, rp_child_node);
    }
    // delete the node
    solRbt_node_free(tree, rp_node);
}

int solRbt_travelsal_inorder(SolRbt *tree, SolRbtNode *node, solRbt_f_ptr_act f)
{
    if (solRbt_node_is_nil(tree, node)) return;
    solRbt_travelsal_inorder(tree, solRbtNode_left(node), f);
    int r = (*f)(tree, node);
    if (r != 0) return r;
    solRbt_travelsal_inorder(tree, solRbtNode_right(node), f);
}

int solRbt_travelsal_preorder(SolRbt *tree, SolRbtNode *node, solRbt_f_ptr_act f)
{
    if (solRbt_node_is_nil(tree, node)) return;
    int r = (*f)(tree, node);
    if (r != 0) return r;
    solRbt_travelsal_preorder(tree, solRbtNode_left(node), f);
    solRbt_travelsal_preorder(tree, solRbtNode_right(node), f);
}


int solRbt_travelsal_backorder(SolRbt *tree, SolRbtNode *node, solRbt_f_ptr_act f)
{
    if (solRbt_node_is_nil(tree, node)) return;
    solRbt_travelsal_backorder(tree, solRbtNode_left(node), f);
    solRbt_travelsal_backorder(tree, solRbtNode_right(node), f);
    int r = (*f)(tree, node);
    if (r != 0) return r;
}

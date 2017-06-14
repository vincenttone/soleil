#include <stdlib.h>
#include "brtree.h"

SolBrt* solBrt_new()
{
    SolBrt *t = sol_calloc(1, sizeof(SolBrt));
    if (t == NULL) {
        return NULL;
    }
    solBrt_set_nil(t, sol_calloc(1, sizeof(SolBrtNode)));
    if (solBrt_nil(t) == NULL) {
        solBrt_free(t);
        return NULL;
    }
    solBrtNode_dye_black(solBrt_nil(t));
    solBrt_set_root(solBrt_nil(t));
    return t;
}

void solBrt_node_free(SolBrt *t, SolBrtNode *n)
{
    if (n) {
        if (solBrt_node_val_free_func(t)) {
            solBrt_node_val_free(solBrtNode_val(n));
        }
        sol_free(n);
    }
}

void solBrt_free(SolBrt *t)
{
    solBrt_travelsal_backorder(t, solBrtNode_root(t), &solBrt_node_free);
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
static int solBrt_left_rorate(SolBrt *tree, SolBrtNode *node)
{
    if (solBrt_node_right_is_nil(tree, node)) {
        // without right child! can not rorate
        return 1;
    }
    // node which chanage to node position
    SolBrtNode *replace2_node = solBrtNode_right(node);
    // change right child's left child to node's right child
    solBrtNode_set_right(node, solBrtNode_left(replace2_node));
    if (solBrt_node_is_NOT_nil(tree, solBrtNode_right(node))) {
        solBrtNode_set_right_parent(node, node);
    }
    // change new node and it's parent relation
    solBrtNode_set_parent(replace2_node, solBrtNode_parent(node));
    if (solBrt_node_is_root(tree, node)) {
        solBrt_set_root(tree, replace2_node);
    } else if (solBrtNode_is_left(node)) {
        solBrtNode_set_parent_left(node, replace2_node);
    } else { // node is right child
        solBrtNode_set_parent_right(node, replace2_node);
    }
    // change node and new node relation
    solBrtNode_set_left(replace2_node, node);
    solBrtNode_set_parent(node, replace2_node);
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
static int solBrt_right_rorate(SolBrt *tree, SolBrtNode *node)
{
    if (solBrt_node_left_is_nil(tree, node)) {
        // without left child! can not rorate
        return 1;
    }
    // the node which chanage to current node position
    SolBrtNode *replace2_node = solBrtNode_left(node);
    // change left child's right child to node's left child
    solBrtNode_set_left(node, solBrtNode_right(replace2_node));
    if (solBrt_node_is_NOT_nil(tree, solBrtNode_left(node))) {
        solBrtNode_set_left_parent(node, node);
    }
    // change new node and it's parent relation
    solBrtNode_set_parent(replace2_node, solBrtNode_parent(node));
    if (solBrt_node_is_root(node)) {
        solBrt_set_root(replace2_node);
    } else if (solBrtNode_is_left(node)) {
        solBrtNode_set_parent_left(node, replace2_node);
    } else { // node is right child
        solBrtNode_set_parent_right(node, replace2_node);
    }
    // change node and new node relation
    solBrtNode_set_right(replace2_node, node);
    solBrtNode_set_parent(node, replace2_node);
    return 0;
}

static void solBrt_insert_fixup(SolBrt *tree, SolBrtNode *node)
{
    // only fixup when parent is red
    while (solBrtNode_parent_is_red(node)) {
        int is_left_branch = 1; // parent is left child 0, right child 1
        SolBrtNode *uncle;
        if (solBrtNode_parent_is_left(node)) {
            // parent is left child
            is_left_branch = 1;
            uncle = solBrtNode_right_elder(node);
        } else {
            is_left_branch = 0;
            uncle = solBrtNode_left_elder(node);
        }
        // cond 1: uncle is red
        if (solBrtNode_is_red(uncle)) {
            // change parent and uncle color to black
            solBrtNode_dye_black(uncle);
            solBrtNode_dye_black(solBrtNode_parent(node));
            // change parent's parent color to red
            solBrtNode_dye_red(solBrtNode_parent(uncle));
            // now insert node is parent's parent
            node = solBrtNode_parent(uncle);
        } else {
            if (is_left_branch) {
                // cond 2: left branch, right child
                if (solBrtNode_is_right(node)) {
                    node = solBrtNode_parent(node);
                    solBrt_left_rorate(tree, node);
                }
                // cond3: left branch, left child
                solBrtNode_dye_black(solBrtNode_parent(node));
                solBrtNode_dye_red(solBrtNode_grandad(node));
                solBrt_right_rorate(tree, solBrtNode_grandad(node));
            } else {
                if (solBrtNode_is_left(node)) {
                    node = solBrtNode_parent(node);
                    solBrt_right_rorate(tree, node);
                }
                solBrtNode_dye_black(solBrtNode_parent(node));
                solBrtNode_dye_red(solBrtNode_grandad(node));
                solBrt_left_rorate(tree, solBrtNode_grandad(node));
            }
        }
    }
    solBrtNode_dye_black(solBrt_root(tree));
}

SolBrtNode* solBrt_insert(SolBrt *tree, void *val)
{
    SolBrtNode *node;
    node = sol_alloc(sizeof(SolBrtNode));
    if (node == NULL) {
        return NULL;
    }
    solBrtNode_dye_red(node);
    solBrtNode_set_weight(node, solBrt_node_val_hash(val));
    solBrtNode_set_parent(node, solBrt_nil(tree));
    solBrtNode_set_left(node, solBrt_nil(tree));
    solBrtNode_set_right(node, solBrt_nil(tree));
    // find insert position
    SolBrtNode *current_node = solBrt_root(tree);
    SolBrtNode *pre_node = solBrt_nil(tree);
    while (solBrt_node_is_NOT_nil(current_node)) {
        pre_node = current_node;
        if (solBrtNode_weight(current_node) == solBrtNode_weight(node)) {
            // has this node
            return;
        } else if (solBrtNode_weight(current_node) > solBrtNode_weight(node)) {
            current_node = solBrtNode_left(current_node);
        } else {
            current_node = solBrtNode_right(current_node);
        }
    }
    solBrtNode_set_parent(node, pre_node);
    if (solBrt_node_is_nil(tree, pre_node)) {
        // empty tree
        solBrt_set_root(tree, node);
    } else if (solBrtNode_weight(node) < solBrtNode_weight(pre_node)) {
        // is left child
        solBrtNode_set_left(pre_node, node);
    } else {
        // is right child
        solBrtNode_set_right(pre_node, node);
    }
    solBrt_insert_fixup(tree, node);
}

/**
 * search the needle key node
 * @params v_br_tree *tree
 * @params long key
 * @return v_br_node || NIL
 *         success: v_br_node
 *         failed: NIL
 */
SolBrtNode* solBrt_search_node(SolBrt *tree, void *val)
{
    size_t weight = solBrt_node_val_hash(val);
    SolBrtNode *current_node = solBrt_root(tree);
    while (solBrt_node_is_NOT_nil(tree, current_node)
           && weight != solBrtNode_weight(current_node)
        ) {
        current_node = weight < solBrtNode_weight(current_node)
            ? solBrtNode_left(current_node)
            : solBrtNode_right(current_node);
    }
    return current_node;
}
/**
 * find min node in node's sub tree
 * @params tree
 * @params node
 * @return min node
 */
SolBrtNode* solBrt_search_min_node(SolBrt *tree, SolBrtNode *node)
{
    if (solBrt_node_is_nil(tree, node)) {
        return solBrt_nil(tree);
    }
    while (solBrt_node_is_NOT_nil(tree, solBrtNode_left(node))) {
        node = solBrtNode_left(node);
    }
    return node;
}
/**
 * find max node int node's sub tree
 */
SolBrtNode* solBrt_search_max_node(SolBrt *tree, SolBrtNode *node)
{
    if (solBrt_node_is_nil(tree, node)) {
        return solBrt_nil(tree);
    }
    while (solBrt_node_is_NOT_nil(tree, solBrtNode_right(node))) {
        node = solBrtNode_right(node);
    }
    return node;
}
/**
 * find in order successor of the node in tree
 * when find in order successor:
 * print out the left node, then the right node
 * just think of this, it should be easy
 */
SolBrtNode* solBrt_search_successor(SolBrt *tree, SolBrtNode *node)
{
    if (solBrt_node_is_nil(tree, node)) {
        return solBrt_nil(tree);
    }
    // has right branch
    // find the min node in right branch
    if (solBrt_node_is_NOT_nil(solBrtNode_right(node))) {
        return solBrt_search_min_node(tree, solBrtNode_right(node));
    }
    // right branch not exists
    // cond 1: node is left child, successor is parent;
    // cond 2: node is right child, find up until match cond 1.
    SolBrtNode *s_node = solBrtNode_parent(node);
    while (solBrt_node_is_NOT_nil(tree, s_node) && node == solBrtNode_right(s_node)) {
        node = s_node;
        s_node = solBrtNode_parent(node);
    }
    return node;
}

/**
 * fixup colors and struct after delete
 * @param v_br_tree
 * @param v_br_node
 * @return void
 */
static void solBrt_delete_fixup(SolBrt *tree, SolBrtNode *node)
{
    while (node != solBrt_root(tree) && solBrtNode_is_black(node)) {
        int is_left_branch = solBrtNode_is_left(node) ? 1 : 0;
        SolBrtNode *brother = is_left_branch
            ? solBrtNode_right(solBrtNode_parent(node))
            : solBrtNode_left(solBrtNode_parent(node));
        if (solBrtNode_is_red(brother)) {
            // when brother is red, convert to black-brother
            solBrtNode_dye_black(brother);
            solBrtNode_dye_red(solBrtNode_parent(node));
            if (is_left_branch) {
                solBrt_left_rorate(tree, solBrtNode_parent(node));
                brother = solBrtNode_right(solBrtNode_parent(node));
            } else {
                solBrt_right_rorate(tree, solBrtNode_parent(node));
                brother = solBrtNode_left(solBrtNode_parent(node));
            }
        }
        // when black-brother
        if (solBrtNode_is_black(solBrtNode_left(brother))
            && solBrtNode_is_black(solBrtNode_right(brother))
            ) {
            // when brother has double-black-children
            // convert to delete_fixup parent node
            solBrtNode_dye_red(brother);
            node = solBrtNode_parent(node);
        } else {
            if (is_left_branch) {
                if (solBrtNode_is_black(solBrtNode_right(brother))) {
                    // when brother has black-right-red-left-children
                    // convert to brother has right-red-child
                    solBrtNode_dye_black(solBrtNode_left(brother));
                    solBrtNode_dye_red(brother);
                    solBrt_right_rorate(tree, brother);
                    brother = solBrtNode_right(solBrtNode_parent(node));
                }
            } else {
                if (solBrtNode_is_black(solBrtNode_left(brother))) {
                    solBrtNode_dye_black(solBrtNode_right(brother));
                    solBrtNode_dye_red(brother);
                    solBrt_left_rorate(tree, brother);
                    brother = solBrtNode_left(solBrtNode_parent(node));
                }
            }
            // when brother has right-red-child (in left branch)
            solBrtNode_dye(brother, solBrtNode_color(solBrtNode_parent(node)));
            solBrtNode_dye_black(solBrtNode_parent(node));
            if (is_left_branch) {
                solBrtNode_dye_black(solBrtNode_right(brother));
                solBrt_left_rorate(tree, solBrtNode_parent(node));
            } else {
                solBrtNode_dye_black(solBrtNode_left(brother));
                solBrt_right_rorate(tree, solBrtNode_parent(node));
            }
            node = solBrt_root(tree);
        }
    }
    // red-black node to black
    solBrtNode_dye_black(node);
}

/**
 * delete node match the key int tree
 */
int solBrt_del(SolBrt *tree, void *val)
{
    SolBrtNode *del_node = solBrt_search_node(tree, val);
    if (solBrt_node_is_nil(tree, del_node)) {
        return 1;
    }
    // find the real position to del
    // cond 1: only one child branch, del the node
    // cond 2: two child branch, find the successor
    SolBrtNode *rp_node = (solBrt_node_is_nil(tree, solBrtNode_left(del_node))
                           || solBrt_node_is_nil(tree, solBrtNode_right(del_node)))
        ? del_node
        : solBrt_search_successor(tree, del_node);
    SolBrtNode *rp_child_node = solBrt_node_is_nil(tree, solBrtNode_left(rp_node))
        ? solBrtNode_right(rp_node)
        : solBrtNode_left(rp_node);
    // adjust relations
    solBrtNode_set_parent(rp_child_node, solBrtNode_parent(rp_node));
    if (solBrt_node_is_root(tree, rp_node)) {
        // rp_node is root
        solBrt_set_root(tree, rp_child_node);
    } else if (solBrtNode_is_left(rp_node)) {
        // rp_node is left child
        solBrtNode_set_left(solBrtNode_parent(rp_node), rp_child_node);
    } else {
        // rp_node is right child
        solBrtNode_set_right(solBrtNode_parent(rp_node), rp_child_node);
    }
    // fix the key
    if (del_node != rp_node) {
        solBrtNode_set_weight(del_node, solBrtNode_weight(rp_node));
        solBrtNode_set_val(del_node, solBrtNode_val(rp_node));
    }
    // if deleted node is black, need to fixup
    if (solBrtNode_is_black(rp_node)) {
        solBrt_delete_fixup(tree, rp_child_node);
    }
    // delete the node
    solBrt_node_free(tree, rp_node);
}

int solBrt_travelsal_inorder(SolBrt *tree, SolBrtNode *node, solBrt_f_ptr_act f)
{
    if (solBrt_node_is_nil(tree, node)) return;
    solBrt_travelsal_inorder(tree, solBrtNode_left(node), f);
    int r = (*f)(tree, node);
    if (r != 0) return r;
    solBrt_travelsal_inorder(tree, solBrtNode_right(node), f);
}

int solBrt_travelsal_preorder(SolBrt *tree, SolBrtNode *node, solBrt_f_ptr_act f)
{
    if (solBrt_node_is_nil(tree, node)) return;
    int r = (*f)(tree, node);
    if (r != 0) return r;
    solBrt_travelsal_preorder(tree, solBrtNode_left(node), f);
    solBrt_travelsal_preorder(tree, solBrtNode_right(node), f);
}


int solBrt_travelsal_backorder(SolBrt *tree, SolBrtNode *node, solBrt_f_ptr_act f)
{
    if (solBrt_node_is_nil(tree, node)) return;
    solBrt_travelsal_backorder(tree, solBrtNode_left(node), f);
    solBrt_travelsal_backorder(tree, solBrtNode_right(node), f);
    int r = (*f)(tree, node);
    if (r != 0) return r;
}

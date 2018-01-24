#ifndef _SOL_BRTREE_H_
#define _SOL_BRTREE_H_ 1

#include "sol_common.h"

enum _SolRBTreeCol {
    _SolRBTreeCol_red = 1,
    _SolRBTreeCol_black,
};

typedef struct _SolRBTreeNode {
    enum _SolRBTreeCol col; //color
    struct _SolRBTreeNode *l; // left
    struct _SolRBTreeNode *r; // right
    struct _SolRBTreeNode *p; // parent
    void *val;
} SolRBTreeNode;

typedef struct _SolRBTree {
    size_t c; // count
    SolRBTreeNode *nil;
    SolRBTreeNode *root;
    sol_f_cmp_ex_ptr f_compare;
    sol_f_free_ptr f_free; // free node val func
    int (*f_insert_conflict_fix)(SolRBTreeNode*, SolRBTreeNode*);
} SolRBTree;

typedef int (*solRBTree_f_ptr_act)(SolRBTree*, SolRBTreeNode*, void*);

SolRBTree* solRBTree_new();
void solRBTree_free(SolRBTree*);
int solRBTree_insert(SolRBTree*, void*);
int solRBTree_delete_node(SolRBTree*, SolRBTreeNode*);
int solRBTree_del(SolRBTree*, void*);
int solRBTree_node_free(SolRBTree*, SolRBTreeNode*);
int _solRBTree_node_free(SolRBTree*, SolRBTreeNode*, void*);

void* solRBTree_search(SolRBTree*, void*);
SolRBTreeNode* solRBTree_search_node(SolRBTree*, void*);
SolRBTreeNode* solRBTree_search_min_node(SolRBTree*, SolRBTreeNode*);
SolRBTreeNode* solRBTree_search_max_node(SolRBTree*, SolRBTreeNode*);
SolRBTreeNode* solRBTree_search_successor(SolRBTree*, SolRBTreeNode*);

void* solRBTree_min(SolRBTree*);
void* solRBTree_max(SolRBTree*);

int solRBTree_left_rorate(SolRBTree*, SolRBTreeNode*);
int solRBTree_right_rorate(SolRBTree*, SolRBTreeNode*);
void solRBTree_insert_fixup(SolRBTree*, SolRBTreeNode*);
void solRBTree_delete_fixup(SolRBTree*, SolRBTreeNode*);

int solRBTree_travelsal_inorder(SolRBTree*, SolRBTreeNode*, solRBTree_f_ptr_act, void*);
int solRBTree_travelsal_preorder(SolRBTree*, SolRBTreeNode*, solRBTree_f_ptr_act, void*);
int solRBTree_travelsal_backorder(SolRBTree*, SolRBTreeNode*, solRBTree_f_ptr_act, void*);

#define solRBTree_root(t) (t)->root
#define solRBTree_nil(t) (t)->nil
#define solRBTree_count(t) (t)->c

#define solRBTree_set_root(t, n) (t)->root = n
#define solRBTree_set_nil(t, n) (t)->nil = n
#define solRBTree_node_is_nil(t, n) ((t)->nil == n)
#define solRBTree_node_is_NOT_nil(t, n) ((t)->nil != n)
#define solRBTree_count_inc(t) (t)->c++
#define solRBTree_count_dec(t) (t)->c--

#define solRBTree_set_val_free_func(t, f) (t)->f_free = f
#define solRBTree_node_val_free_func(t) (t)->f_free
#define solRBTree_node_val_free(v) (*(t)->f_free)(v)

#define solRBTree_set_insert_conflict_fix_func(t, f) (t)->f_insert_conflict_fix = f
#define solRBTree_insert_conflict_fix_func(t) (t)->f_insert_conflict_fix
#define solRBTree_insert_conflict_fix(t, n1, n2) (*(t)->f_insert_conflict_fix)(n1, n2)

#define solRBTree_set_compare_func(t, f) (t)->f_compare = f
#define solRBTree_node_val_compare_func(t) (t)->f_compare
#define solRBTree_node_val_compare(t, v1, v2, f) (*(t)->f_compare)(v1, v2, f)
#define solRBTree_node_compare(t, n1, n2) solRBTree_node_val_compare(t, solRBTreeNode_val(n1), solRBTreeNode_val(n2))

#define solRBTreeNode_left(n) n->l
#define solRBTreeNode_right(n) n->r
#define solRBTreeNode_parent(n) n->p
#define solRBTreeNode_val(n) n->val
#define solRBTreeNode_color(n) n->col

#define solRBTreeNode_set_left(n, x) n->l = x
#define solRBTreeNode_set_right(n, x) n->r = x
#define solRBTreeNode_set_parent(n, x) n->p = x
#define solRBTreeNode_set_val(n, v) n->val = v
#define solRBTreeNode_set_color(n, x) n->col = x

#define solRBTreeNode_dye(n, x) solRBTreeNode_set_color(n, x)
#define solRBTreeNode_dye_red(n) solRBTreeNode_set_color(n, _SolRBTreeCol_red)
#define solRBTreeNode_dye_black(n) solRBTreeNode_set_color(n, _SolRBTreeCol_black)

#define solRBTreeNode_is_red(n) solRBTreeNode_color(n) == _SolRBTreeCol_red
#define solRBTreeNode_is_black(n) solRBTreeNode_color(n) == _SolRBTreeCol_black

#define solRBTreeNode_grandad(n) solRBTreeNode_parent(solRBTreeNode_parent(n))
#define solRBTreeNode_left_elder(n) solRBTreeNode_left(solRBTreeNode_grandad(n))
#define solRBTreeNode_right_elder(n) solRBTreeNode_right(solRBTreeNode_grandad(n))

#define solRBTree_node_is_root(t, n) solRBTree_node_is_nil(t, solRBTreeNode_parent(n))
#define solRBTreeNode_is_left(n) (solRBTreeNode_left(solRBTreeNode_parent(n)) == n)
#define solRBTreeNode_is_right(n) (solRBTreeNode_right(solRBTreeNode_parent(n)) == n)
#define solRBTreeNode_parent_is_left(n) (solRBTreeNode_left_elder(n) == solRBTreeNode_parent(n))
#define solRBTreeNode_parent_is_right(n) (solRBTreeNode_right_elder(n) == solRBTreeNode_parent(n))
#define solRBTreeNode_parent_is_red(n) solRBTreeNode_is_red(solRBTreeNode_parent(n))

#define solRBTreeNode_set_left_parent(n, x) solRBTreeNode_set_parent(solRBTreeNode_left(n), x)
#define solRBTreeNode_set_right_parent(n, x) solRBTreeNode_set_parent(solRBTreeNode_right(n), x)
#define solRBTreeNode_set_parent_left(n, x) solRBTreeNode_set_left(solRBTreeNode_parent(n), x)
#define solRBTreeNode_set_parent_right(n, x) solRBTreeNode_set_right(solRBTreeNode_parent(n), x)

#define solRBTree_node_left_is_nil(t, n) solRBTree_node_is_nil(t, solRBTreeNode_left(n))
#define solRBTree_node_right_is_nil(t, n) solRBTree_node_is_nil(t, solRBTreeNode_right(n))

#endif

#ifndef _SOL_BRTREE_H_
#define _SOL_BRTREE_H_ 1

#include "sol_common.h"

typedef int (*solRbt_f_ptr_act)(SolRbt*, SolRbtNode*);
typedef int (*solRbt_f_ptr_insert)(SolRbt*, SolRbtNode*);

enum _SolRbtCol {
    _SolRbtCol_red = 1,
    _SolRbtCol_black,
};

typedef struct _SolRbtNode {
    size_t w; // weight
    enum _SolRbtCol col; //color
    struct _SolRbtNode *l; // left
    struct _SolRbtNode *r; // right
    struct _SolRbtNode *p; // parent
    void *val;
} SolRbtNode;

typedef struct _SolRbt {
    SolRbtNode *nil;
    SolRbtNode *root;
    sol_f_match_ptr f_match;
    sol_f_hash_ptr f_hash; // get weight
    sol_f_free_ptr f_free; // free node val func
    solRbt_f_ptr_insert f_insert;
} SolRbt;

SolRbt* solRbt_new();
void solRbt_free(SolRbt*);
SolRbtNode* solRbt_insert(SolRbt*, void*);
int solRbt_del(SolRbt*, void*);
void solRbt_node_free(SolRbt*, SolRbtNode*);

SolRbtNode* solRbt_search_node(SolRbt*, void*);
SolRbtNode* solRbt_search_min_node(SolRbt*, void*);
SolRbtNode* solRbt_search_max_node(SolRbt*, void*);
SolRbtNode* solRbt_search_successor(SolRbt*, SolRbtNode*);

static int solRbt_left_rorate(SolRbt*, SolRbtNode*);
static int solRbt_right_rorate(SolRbt*, SolRbtNode*);
static void solRbt_insert_fixup(SolRbt*, SolRbtNode*);
static void solRbt_delete_fixup(SolRbt*, SolRbtNode*);

int solRbt_travelsal_inorder(SolRbt*, SolRbtNode*, solRbt_f_ptr_act);
int solRbt_travelsal_preorder(SolRbt*, SolRbtNode*, solRbt_f_ptr_act);
int solRbt_travelsal_backorder(SolRbt*, SolRbtNode*, solRbt_f_ptr_act);

#define solRbt_root(t) t->root
#define solRbt_nil(t) t->nil

#define solRbt_set_root(t, n) t->root = n
#define solRbt_set_nil(t, n) t->nil = n
#define solRbt_node_is_nil(t, n) (t->nil == n)
#define solRbt_node_is_NOT_nil(t, n) (t->nil != n)

#define solRbt_node_val_free_func(t) t->f_free
#define solRbt_node_val_free(v) (*t->f_free)(v)

#define solRbt_node_val_hash_func(t) t->f_hash
#define solRbt_node_val_hash(v) (*t->f_hash)(v)

#define solRbtNode_left(n) n->l
#define solRbtNode_right(n) n->r
#define solRbtNode_parent(n) n->p
#define solRbtNode_val(n) n->val
#define solRbtNode_color(n) n->col
#define solRbtNode_weight(n) n->w

#define solRbtNode_set_left(n, x) n->l = x
#define solRbtNode_set_right(n, x) n->r = x
#define solRbtNode_set_parent(n, x) n->p = x
#define solRbtNode_set_val(n, v) n->val = v
#define solRbtNode_set_color(n, x) n->col = x
#define solRbtNode_set_weight(n, x) n->w = x

#define solRbtNode_dye(n, x) solRbtNode_set_color(n, x)
#define solRbtNode_dye_red(n) solRbtNode_set_color(n, _SolRbtCol_red)
#define solRbtNode_dye_black(n) solRbtNode_set_color(n, _SolRbtCol_black)

#define solRbtNode_is_red(n) solRbtNode_color(n) == _SolRbtCol_red
#define solRbtNode_is_black(n) solRbtNode_color(n) == _SolRbtCol_black

#define solRbtNode_grandad(n) solRbtNode_parent(solRbtNode_parent(n))
#define solRbtNode_left_elder(n) solRbtNode_left(solRbtNode_grandad(n))
#define solRbtNode_right_elder(n) solRbtNode_right(solRbtNode_grandad(n))

#define solRbt_node_is_root(t, n) solRbt_node_is_nil(t, solRbtNode_parent(n))
#define solRbtNode_is_left(n) solRbtNode_left(solRbtNode_parent(n)) == n
#define solRbtNode_is_right(n) solRbtNode_right(solRbtNode_parent(n)) == n
#define solRbtNode_parent_is_left(n) solRbtNode_left_elder(n) == solRbtNode_parent(n)
#define solRbtNode_parent_is_right(n) solRbtNode_right_elder(n) == solRbtNode_parent(n)
#define solRbtNode_parent_is_red(n) solRbtNode_is_red(solRbtNode_parent(n))

#define solRbtNode_set_left_parent(n, x) solRbtNode_set_parent(solRbtNode_left(n), x)
#define solRbtNode_set_right_parent(n, x) solRbtNode_set_parent(solRbtNode_right(n), x)
#define solRbtNode_set_parent_left(n, x) solRbtNode_set_left(solRbtNode_parent(n), x)
#define solRbtNode_set_parent_right(n, x) solRbtNode_set_right(solRbtNode_parent(n), x)

#define solRbt_node_left_is_nil(t, n) solRbt_node_is_nil(t, solRbtNode_left(n))
#define solRbt_node_right_is_nil(t, n) solRbt_node_is_nil(t, solRbtNode_right(n))

#endif

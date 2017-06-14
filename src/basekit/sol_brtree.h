#ifndef _SOL_BRTREE_H_
#define _SOL_BRTREE_H_ 1

#include "sol_common.h"

typedef int (*solBrt_f_ptr_act)(SolBrt*, SolBrtNode*);
typedef int (*solBrt_f_ptr_insert)(SolBrt*, SolBrtNode*);

enum _SolBrtCol {
    _SolBrtCol_red = 1,
    _SolBrtCol_black,
};

typedef struct _SolBrtNode {
    size_t w; // weight
    enum _SolBrtCol col; //color
    struct _SolBrtNode *l; // left
    struct _SolBrtNode *r; // right
    struct _SolBrtNode *p; // parent
    void *val;
} SolBrtNode;

typedef struct _SolBrt {
    SolBrtNode *nil;
    SolBrtNode *root;
    sol_f_match_ptr f_match;
    sol_f_hash_ptr f_hash; // get weight
    sol_f_free_ptr f_free; // free node val func
    solBrt_f_ptr_insert f_insert;
} SolBrt;

SolBrt* solBrt_new();
void solBrt_free(SolBrt*);
SolBrtNode* solBrt_insert(SolBrt*, void*);
int solBrt_del(SolBrt*, void*);
void solBrt_node_free(SolBrt*, SolBrtNode*);

SolBrtNode* solBrt_search_node(SolBrt*, void*);
SolBrtNode* solBrt_search_min_node(SolBrt*, void*);
SolBrtNode* solBrt_search_max_node(SolBrt*, void*);
SolBrtNode* solBrt_search_successor(SolBrt*, SolBrtNode*);

static int solBrt_left_rorate(SolBrt*, SolBrtNode*);
static int solBrt_right_rorate(SolBrt*, SolBrtNode*);
static void solBrt_insert_fixup(SolBrt*, SolBrtNode*);
static void solBrt_delete_fixup(SolBrt*, SolBrtNode*);

int solBrt_travelsal_inorder(SolBrt*, SolBrtNode*, solBrt_f_ptr_act);
int solBrt_travelsal_preorder(SolBrt*, SolBrtNode*, solBrt_f_ptr_act);
int solBrt_travelsal_backorder(SolBrt*, SolBrtNode*, solBrt_f_ptr_act);

#define solBrt_root(t) t->root
#define solBrt_nil(t) t->nil

#define solBrt_set_root(t, n) t->root = n
#define solBrt_set_nil(t, n) t->nil = n
#define solBrt_node_is_nil(t, n) (t->nil == n)
#define solBrt_node_is_NOT_nil(t, n) (t->nil != n)

#define solBrt_node_val_free_func(t) t->f_free
#define solBrt_node_val_free(v) (*t->f_free)(v)

#define solBrt_node_val_hash_func(t) t->f_hash
#define solBrt_node_val_hash(v) (*t->f_hash)(v)

#define solBrtNode_left(n) n->l
#define solBrtNode_right(n) n->r
#define solBrtNode_parent(n) n->p
#define solBrtNode_val(n) n->val
#define solBrtNode_color(n) n->col
#define solBrtNode_weight(n) n->w

#define solBrtNode_set_left(n, x) n->l = x
#define solBrtNode_set_right(n, x) n->r = x
#define solBrtNode_set_parent(n, x) n->p = x
#define solBrtNode_set_val(n, v) n->val = v
#define solBrtNode_set_color(n, x) n->col = x
#define solBrtNode_set_weight(n, x) n->w = x

#define solBrtNode_dye(n, x) solBrtNode_set_color(n, x)
#define solBrtNode_dye_red(n) solBrtNode_set_color(n, _SolBrtCol_red)
#define solBrtNode_dye_black(n) solBrtNode_set_color(n, _SolBrtCol_black)

#define solBrtNode_is_red(n) solBrtNode_color(n) == _SolBrtCol_red
#define solBrtNode_is_black(n) solBrtNode_color(n) == _SolBrtCol_black

#define solBrtNode_grandad(n) solBrtNode_parent(solBrtNode_parent(n))
#define solBrtNode_left_elder(n) solBrtNode_left(solBrtNode_grandad(n))
#define solBrtNode_right_elder(n) solBrtNode_right(solBrtNode_grandad(n))

#define solBrt_node_is_root(t, n) solBrt_node_is_nil(t, solBrtNode_parent(n))
#define solBrtNode_is_left(n) solBrtNode_left(solBrtNode_parent(n)) == n
#define solBrtNode_is_right(n) solBrtNode_right(solBrtNode_parent(n)) == n
#define solBrtNode_parent_is_left(n) solBrtNode_left_elder(n) == solBrtNode_parent(n)
#define solBrtNode_parent_is_right(n) solBrtNode_right_elder(n) == solBrtNode_parent(n)
#define solBrtNode_parent_is_red(n) solBrtNode_is_red(solBrtNode_parent(n))

#define solBrtNode_set_left_parent(n, x) solBrtNode_set_parent(solBrtNode_left(n), x)
#define solBrtNode_set_right_parent(n, x) solBrtNode_set_parent(solBrtNode_right(n), x)
#define solBrtNode_set_parent_left(n, x) solBrtNode_set_left(solBrtNode_parent(n), x)
#define solBrtNode_set_parent_right(n, x) solBrtNode_set_right(solBrtNode_parent(n), x)

#define solBrt_node_left_is_nil(t, n) solBrt_node_is_nil(t, solBrtNode_left(n))
#define solBrt_node_right_is_nil(t, n) solBrt_node_is_nil(t, solBrtNode_right(n))

#endif

#ifndef _SOL_RBTREE_ITER_H_
#define _SOL_RBTREE_ITER_H_ 1

#include "sol_common.h"
#include "sol_rbtree.h"
#include "sol_stack.h"

typedef enum _SolRBTreeIterTravelsalType {
    SolRBTreeIterTT_preorder = 1,
    SolRBTreeIterTT_inorder,
    SolRBTreeIterTT_backorder,
} SolRBTreeIterTravelsalType;

typedef struct _SolRBTreeIter {
    SolRBTreeIterTravelsalType tt;
    SolRBTree *t;
    SolRBTreeNode *n;
    SolRBTreeNode *cn;
    SolStack *s;
} SolRBTreeIter;

#define solRBTreeIter_preorder_new(t, n) solRBTreeIter_new(t, n, SolRBTreeIterTT_preorder)
#define solRBTreeIter_inorder_new(t, n) solRBTreeIter_new(t, n, SolRBTreeIterTT_inorder)
#define solRBTreeIter_backorder_new(t, n) solRBTreeIter_new(t, n, SolRBTreeIterTT_backorder)

SolRBTreeIter* solRBTreeIter_new(SolRBTree*, SolRBTreeNode*, SolRBTreeIterTravelsalType);
SolRBTreeNode* solRBTreeIter_current(SolRBTreeIter*);
void* solRBTreeIter_current_val(SolRBTreeIter*);
SolRBTreeNode* solRBTreeIter_next(SolRBTreeIter*);
void* solRBTreeIter_next_val(SolRBTreeIter*);
void solRBTreeIter_free(SolRBTreeIter*);
void solRBTreeIter_reset(SolRBTreeIter*);

SolRBTreeNode* solRBTreeIter_next_preorder(SolRBTreeIter*);
SolRBTreeNode* solRBTreeIter_next_inorder(SolRBTreeIter*);
SolRBTreeNode* solRBTreeIter_next_backorder(SolRBTreeIter*);

#endif

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

SolRBTreeIter* solRBTreeIter_new(SolRBTree*, SolRBTreeNode*, SolRBTreeIterTravelsalType);
SolRBTreeNode* solRBTreeIter_current(SolRBTreeIter*);
void* solRBTreeIter_current_val(SolRBTreeIter*);
SolRBTreeNode* solRBTreeIter_next(SolRBTreeIter*);
void* solRBTreeIter_next_val(SolRBTreeIter*);
void solRBTreeIter_free(SolRBTreeIter*);

SolRBTreeNode* solRBTreeIter_next_preorder(SolRBTreeIter*);
SolRBTreeNode* solRBTreeIter_next_inorder(SolRBTreeIter*);
SolRBTreeNode* solRBTreeIter_next_backorder(SolRBTreeIter*);

#endif

#include "sol_rbtree_iter.h"

SolRBTreeIter* solRBTreeIter_new(SolRBTree *t, SolRBTreeNode *n, SolRBTreeIterTravelsalType tt)
{
    SolRBTreeIter *i = sol_alloc(sizeof(SolRBTreeIter));
    if (i == NULL) return NULL;
    i->s = solStack_new();
    if (i->s == NULL) {
        solRBTreeIter_free(i);
        return NULL;
    }
    i->tt = tt;
    i->t = t;
    i->n = n;
    return i;
}

void solRBTreeIter_free(SolRBTreeIter *i)
{
    if (i->s) {
        solStack_free(i->s);
    }
    sol_free(i);
}

SolRBTreeNode* solRBTreeIter_current(SolRBTreeIter *i)
{
    if (i == NULL) return NULL;
    return i->n;
}

void* solRBTreeIter_current_val(SolRBTreeIter *i)
{
    if (i == NULL || i->n == NULL) return NULL;
    return solRBTreeNode_val(i->n);
}

SolRBTreeNode* solRBTreeIter_next(SolRBTreeIter *i)
{
parent:
    if (solRBTreeNode_left(i->n)) {
        goto right;
    } if (solRBTreeNode_right(i->n)) {
        i->n = solRBTreeNode_right(i->n);
        goto left;
    }
    solStack_pop(i->n);
left:
    if (solRBTreeNode_left(i->n)) {
        i->n = solRBTreeNode_left(i->n);
        goto left;
    }
    goto parent;
right:
    i->n = solRBTreeNode_right(i->n);
    goto left;
}

void* solRBTreeIter_next_val(SolRBTreeIter *i)
{
    solRBTreeIter_next(i);
    return solRBTreeIter_current_val(i);
}

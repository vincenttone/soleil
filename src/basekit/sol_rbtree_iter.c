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
    i->cn = NULL;
    return i;
}

void solRBTreeIter_free(SolRBTreeIter *i)
{
    if (i->s) {
        solStack_free(i->s);
    }
    sol_free(i);
}

void solRBTreeIter_reset(SolRBTreeIter *i)
{
    SolRBTreeNode *n;
    do {
        n = solStack_pop(i->s);
    } while ((n && solRBTree_node_is_NOT_nil(i->t, n)));
    i->cn = NULL;
}

SolRBTreeNode* solRBTreeIter_current(SolRBTreeIter *i)
{
    if (i == NULL) return NULL;
    return i->cn;
}

void* solRBTreeIter_current_val(SolRBTreeIter *i)
{
    if (i == NULL || i->cn == NULL) return NULL;
    return solRBTreeNode_val(i->cn);
}

SolRBTreeNode* solRBTreeIter_next(SolRBTreeIter *i)
{
    switch (i->tt) {
    case SolRBTreeIterTT_preorder:
        return solRBTreeIter_next_preorder(i);
    case SolRBTreeIterTT_inorder:
        return solRBTreeIter_next_inorder(i);
    case SolRBTreeIterTT_backorder:
        return solRBTreeIter_next_backorder(i);
    }
    return NULL;
}

SolRBTreeNode* solRBTreeIter_next_preorder(SolRBTreeIter *i)
{
    if (solRBTree_node_is_nil(i->t, i->n)) {
        return NULL;
    }
    SolRBTreeNode *n;
    n = solStack_pop(i->s);
    if (n == NULL && i->cn == NULL) {
        n = i->n;
    }
    if (n && solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_right(n))) {
        solStack_push(i->s, solRBTreeNode_right(n));
    }
    if (n && solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_left(n))) {
        solStack_push(i->s, solRBTreeNode_left(n));
    }
    i->cn = n;
    return n;
}

SolRBTreeNode* solRBTreeIter_next_inorder(SolRBTreeIter *i)
{
    if (solRBTree_node_is_nil(i->t, i->n)) {
        return NULL;
    }
    SolRBTreeNode *n;
    n = solStack_pop(i->s);
    if (n == NULL && i->cn == NULL) {
        n = i->n;
        do {
            solStack_push(i->s, n);
            n = solRBTreeNode_left(n);
        } while (solRBTree_node_is_NOT_nil(i->t, n));
        i->cn = solStack_pop(i->s);
    } else if (n && solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_right(n))) {
        i->cn = n;
        n = solRBTreeNode_right(n);
        do {
            solStack_push(i->s, n);
            n = solRBTreeNode_left(n);
        } while (solRBTree_node_is_NOT_nil(i->t, n));
    } else {
        i->cn = n;
    }
    return i->cn;
}

SolRBTreeNode* solRBTreeIter_next_backorder(SolRBTreeIter *i)
{
    if (solRBTree_node_is_nil(i->t, i->n)) {
        return NULL;
    }
    SolRBTreeNode *n;
    n = solStack_pop(i->s);
    if (n == NULL && i->cn == NULL) {
        SolStack *s = solStack_new();
        solStack_push(s, i->n);
        while ((n = solStack_pop(s))) {
            solStack_push(i->s, n);
            if (solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_left(n))) {
                solStack_push(s, solRBTreeNode_left(n));
            }
            if (solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_right(n))) {
                solStack_push(s, solRBTreeNode_right(n));
            }
        }
        solStack_free(s);
        i->cn = solStack_pop(i->s);
    } else {
        i->cn = n;
    }
    return i->cn;
}

void* solRBTreeIter_next_val(SolRBTreeIter *i)
{
    solRBTreeIter_next(i);
    return solRBTreeIter_current_val(i);
}

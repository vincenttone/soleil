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
    switch (tt) {
    case SolRBTreeIterTT_preorder:
        i->cn = n;
        break;
    case SolRBTreeIterTT_inorder:
        i->cn = n;
        while (solRBTree_node_is_NOT_nil(t, solRBTreeNode_left(i->cn))) {
            solStack_push(i->s, i->cn);
            i->cn = solRBTreeNode_left(i->cn);
        }
        break;
    case SolRBTreeIterTT_backorder:
        i->cn = n;
    check_children:
        while (solRBTree_node_is_NOT_nil(t, solRBTreeNode_left(i->cn))) {
            solStack_push(i->s, i->cn);
            i->cn = solRBTreeNode_left(i->cn);
        }
        if (solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_right(i->cn))) {
            solStack_push(i->s, i->cn);
            i->cn = solRBTreeNode_right(i->cn);
        }
        if (solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_left(i->cn))) {
            goto check_children;
        }
        break;
    }
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
    if (i == NULL
        || i->cn == NULL
        || solRBTree_node_is_nil(i->t, i->cn)
        ) return NULL;
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
    if (i == NULL
        || i->n == NULL
        || i->cn == NULL
        || solRBTree_node_is_nil(i->t, i->n)
        || solRBTree_node_is_nil(i->t, i->cn)
        ) {
        return NULL;
    }
    if (solStack_size(i->s) > 0) {
        i->cn = solStack_pop(i->s);
    } else {
        solStack_push(i->s, solRBTreeNode_right(i->cn));
        if (solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_left(i->cn))) {
            i->cn = solRBTreeNode_left(i->cn);            
        } else {
            i->cn = solStack_pop(i->s);
        }
    }
    if (solRBTree_node_is_NOT_nil(i->t, i->cn)) {
        if (solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_right(i->cn))) {
            solStack_push(i->s, solRBTreeNode_right(i->cn));
        }
        if (solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_left(i->cn))) {
            solStack_push(i->s, solRBTreeNode_left(i->cn));
        }
    } else {
        return NULL;
    }
    return i->cn;
}

SolRBTreeNode* solRBTreeIter_next_inorder(SolRBTreeIter *i)
{
    if (i == NULL
        || i->n == NULL
        || i->cn == NULL
        || solRBTree_node_is_nil(i->t, i->n)
        ) {
        return NULL;
    }
    if (solStack_size(i->s) > 0) {
        i->cn = solStack_pop(i->s);
        SolRBTreeNode *n = i->cn;
        if (solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_right(n))) {
            n = solRBTreeNode_right(i->cn);
            do {
                solStack_push(i->s, n);
                n = solRBTreeNode_left(n);
            } while (solRBTree_node_is_NOT_nil(i->t, n));
        }
    } else {
        return NULL;
    }
    return i->cn;
}

SolRBTreeNode* solRBTreeIter_next_backorder(SolRBTreeIter *i)
{
    if (solRBTree_node_is_nil(i->t, i->n)) {
        return NULL;
    }
    if (i->cn != i->n
        && solRBTreeNode_is_left(i->cn)
        && solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_right(solRBTreeNode_parent(i->cn)))
        ) {
        i->cn = solRBTreeNode_right(solRBTreeNode_parent(i->cn));
    check_children:
        while (solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_left(i->cn))) {
            solStack_push(i->s, i->cn);
            i->cn = solRBTreeNode_left(i->cn);
        }
        if (solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_right(i->cn))) {
            solStack_push(i->s, i->cn);
            i->cn = solRBTreeNode_right(i->cn);
        }
        if (solRBTree_node_is_NOT_nil(i->t, solRBTreeNode_left(i->cn))) {
            goto check_children;
        }
    } else if (solStack_size(i->s) > 0) {
        i->cn = solStack_pop(i->s);
    } else {
        return NULL;
    }
    return i->cn;
}

void* solRBTreeIter_next_val(SolRBTreeIter *i)
{
    solRBTreeIter_next(i);
    return solRBTreeIter_current_val(i);
}

#include <assert.h>
#include "sol_list.h"

SolList* solList_new()
{
    SolList *l = sol_calloc(1, sizeof(SolList));
    return l;
}
void solList_free(SolList *l)
{
    SolListNode *n = solList_head(l);
    SolListNode *nn;
    while (n) {
        nn = solListNode_next(n);
        solListNode_free(l, n);
        n = nn;
    }
    sol_free(l);
}

SolListNode* solList_add(SolList *l, void *v)
{
    SolListNode *n = solListNode_new(v);
    if (n == NULL) {
        return NULL;
    }
    if (solList_len(l) == 0) {
        solList_set_head(l, n);
        solList_set_tail(l, n);
    } else {
        solListNode_set_next(solList_tail(l), n);
        solList_set_tail(l, n);
    }
    solList_incr_len(l);
    return n;
}

int solList_has(SolList *l, void *v)
{
    SolListNode *n = solList_head(l);
    if (n == NULL) {
        return -1;
    }
    SolListNode *nn;
    while (n) {
        nn = solListNode_next(n);
        if (solListVal_match_func(l)) {
            if (solListVal_match(l, solListNode_val(n), v) == 0) {
                return 0;
            }
        } else {
            if (solListNode_val(n) == v) {
                return 0;
            }
        }
        n = nn;
    }
    return 1;
}

int solList_del_node(SolList *l, SolListNode *n)
{
    SolListNode *cn = solList_head(l);
    if (cn == NULL) {
        return -1;
    }
    SolListNode *pn = NULL;
    while (cn) {
        if (cn == n) {
            if (n == solList_head(l)) {
                solList_set_head(l, solListNode_next(cn));
            } else {
                solListNode_set_next(pn, solListNode_next(cn));
            }
            if (n == solList_tail(l)) {
                solList_set_tail(l, pn);
            }
            solListNode_free(l, n);
            return 0;
        }
        pn = cn;
        cn = solListNode_next(cn);
    }
    return 1;
}

int solList_remove(SolList *l, void *v)
{
    SolListNode *n = solList_head(l);
    SolListNode *pn = NULL;
    SolListNode *nn;
    while (n) {
        nn = solListNode_next(n);
        if (solListVal_match_func(l)) {
            if (solListVal_match(l, solListNode_val(n), v) == 0) {
                if (pn == NULL) {
                    solList_set_head(l, nn);
                } else {
                    solListNode_set_next(pn, nn);
                }
                if (nn == NULL) {
                    solList_set_tail(l, NULL);
                }
                solListNode_free(l, n);
            }
        } else {
            if (solListNode_val(n) == v) {
                if (pn == NULL) {
                    solList_set_head(l, nn);
                } else {
                    solListNode_set_next(pn, nn);
                }
                if (nn == NULL) {
                    solList_set_tail(l, NULL);
                }
                solListNode_free(l, n);
            }
        }
        pn = n;
        n = nn;
    }
    return 1;
}

int solList_uniq(SolList *l)
{
    SolListNode *n = solList_head(l);
    if (n == NULL) return -1;
    SolListNode *n1;
    SolListNode *pn;
    SolListNode *nn;
    do {
        n1 = n;
        pn = n;
        nn = solListNode_next(n1);
        while ((n1 = nn)) {
            nn = solListNode_next(n1);
            if (solListVal_match_func(l)) {
                if (solListVal_match(l, solListNode_val(n), solListNode_val(n1)) == 0) {
                    goto del_node;
                }
            } else {
                if (solListNode_val(n) == solListNode_val(n1)) {
                    goto del_node;
                }
            }
            pn = n1;
            continue;
        del_node:
            if (solList_tail(l) == n1) {
                solList_set_tail(l, pn);
            }
            solListNode_set_next(pn, nn);
            solListNode_free(l, n1);
        }
    } while ((n = solListNode_next(n)));
    return 0;
}

int solList_attach(SolList *l1, SolList *l2)
{
    if (l1 == NULL || l2 == NULL) {
        return -1;
    }
    if (solList_head(l1) == NULL) {
        solList_set_head(l1, solList_head(l2));
        goto finish;
    }
    assert(solList_tail(l1) && "seems like slist do not have tail");
    solListNode_set_next(solList_tail(l1), solList_head(l2));
finish:
    solList_set_tail(l1, solList_tail(l2));
    l1->len += l2->len;
    solList_set_head(l2, NULL);
    solList_set_tail(l2, NULL);
    solList_set_len(l2, 0);
    solList_free(l2);
    return 0;
}

int solList_merge(SolList *l1, SolList *l2)
{
    if (l1 == NULL || l2 == NULL)
        return -1;
    if (solList_len(l2) == 0) return 0;
    SolListNode *n = solList_head(l2);
    do {
        solList_add(l1, solListNode_val(n));
    } while ((n = solListNode_next(n)));
    return 0;
}

SolList* solList_dup(SolList *l)
{
    if (l == NULL) return NULL;
    SolList *l1 = solList_new();
    if (solList_len(l) == 0) return l1;
    SolListNode *n = solList_head(l);
    do {
        solList_add(l1, solListNode_val(n));
        n = solListNode_next(n);
    } while (n);
    return l1;
}

SolListNode* solListNode_new(void *val)
{
    SolListNode *n = sol_calloc(1, sizeof(SolListNode));
    if (n == NULL) {
        return NULL;
    }
    solListNode_set_val(n, val);
    return n;
}

void solListNode_free(SolList *l, SolListNode *n)
{
    if (solListVal_free_func(l)) {
        solListVal_free(l, solListNode_val(n));
    }
    sol_free(n);
}

SolListIter* solListIter_new(SolList *l)
{
    if (l == NULL) return NULL;
    SolListIter *i = sol_alloc(sizeof(SolListIter));
    if (i == NULL) return NULL;
    i->l = l;
    i->n = solList_head(l);
    return i;
}

void solListIter_free(SolListIter *i)
{
    if (i) sol_free(i);
}

SolListNode* solListIter_current(SolListIter *i)
{
    if (i == NULL || i->n == NULL)
        return NULL;
    return i->n;
}

SolListNode* solListIter_next(SolListIter *i)
{
    if (i == NULL || i->n == NULL)
        return NULL;
    i->n = solListNode_next(i->n);
    return i->n;
}

void* solListIter_current_val(SolListIter *i)
{
    if (i == NULL || i->n == NULL)
        return NULL;
    return solListNode_val(i->n);
}

void* solListIter_next_val(SolListIter *i)
{
    if (i == NULL || i->n == NULL) return NULL;
    i->n = solListNode_next(i->n);
    if (i->n == NULL) return NULL;
    return solListNode_val(i->n);
}

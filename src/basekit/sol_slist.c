#include <assert.h>
#include "sol_slist.h"

SolSlist* solSlist_new()
{
    SolSlist *l = sol_calloc(1, sizeof(SolSlist));
    return l;
}
void solSlist_free(SolSlist *l)
{
    SolSlistNode *n = solSlist_head(l);
    SolSlistNode *nn;
    while (n) {
        nn = solSlistNode_next(n);
        solSlistNode_free(l, n);
        n = nn;
    }
    sol_free(l);
}

SolSlistNode* solSlist_add(SolSlist *l, void *v)
{
    SolSlistNode *n = solSlistNode_new(v);
    if (n == NULL) {
        return NULL;
    }
    if (solSlist_len(l) == 0) {
        solSlist_set_head(l, n);
        solSlist_set_tail(l, n);
    } else {
        solSlistNode_set_next(solSlist_tail(l), n);
        solSlist_set_tail(l, n);
    }
    solSlist_incr_len(l);
    return n;
}

int solSlist_has(SolSlist *l, void *v)
{
    SolSlistNode *n = solSlist_head(l);
    if (n == NULL) {
        return -1;
    }
    SolSlistNode *nn;
    while (n) {
        nn = solSlistNode_next(n);
        if (solSlistVal_match_func(l)) {
            if (solSlistVal_match(l, solSlistNode_val(n), v) == 0) {
                return 0;
            }
        } else {
            if (solSlistNode_val(n) == v) {
                return 0;
            }
        }
        n = nn;
    }
    return 1;
}

int solSlist_del_node(SolSlist *l, SolSlistNode *n)
{
    SolSlistNode *cn = solSlist_head(l);
    if (cn == NULL) {
        return -1;
    }
    SolSlistNode *pn = NULL;
    SolSlistNode *nn;
    while (cn) {
        nn = solSlistNode_next(cn);
        if (cn == n) {
            if (pn == NULL) {
                solSlist_set_head(l, nn);
            } else {
                solSlistNode_set_next(pn, nn);
            }
            if (nn == NULL) {
                solSlist_set_tail(l, NULL);
            }
            solSlistNode_free(l, n);
            return 0;
        }
        pn = cn;
        cn = nn;
    }
    return 1;
}

int solSlist_remove(SolSlist *l, void *v)
{
    SolSlistNode *n = solSlist_head(l);
    SolSlistNode *pn = NULL;
    SolSlistNode *nn;
    while (n) {
        nn = solSlistNode_next(n);
        if (solSlistVal_match_func(l)) {
            if (solSlistVal_match(l, solSlistNode_val(n), v) == 0) {
                if (pn == NULL) {
                    solSlist_set_head(l, nn);
                } else {
                    solSlistNode_set_next(pn, nn);
                }
                if (nn == NULL) {
                    solSlist_set_tail(l, NULL);
                }
                solSlistNode_free(l, n);
            }
        } else {
            if (solSlistNode_val(n) == v) {
                if (pn == NULL) {
                    solSlist_set_head(l, nn);
                } else {
                    solSlistNode_set_next(pn, nn);
                }
                if (nn == NULL) {
                    solSlist_set_tail(l, NULL);
                }
                solSlistNode_free(l, n);
            }
        }
        pn = n;
        n = nn;
    }
    return 1;
}

int solSlist_merge(SolSlist *l1, SolSlist *l2)
{
    if (l1 == NULL || l2 == NULL) {
        return -1;
    }
    if (solSlist_head(l1) == NULL) {
        solSlist_set_head(l1, solSlist_head(l2));
        goto finish;
    }
    assert(solSlist_tail(l1) && "seems like slist do not have tail");
    solSlistNode_set_next(solSlist_tail(l1), solSlist_head(l2));
finish:
    solSlist_set_tail(l1, solSlist_tail(l2));
    l1->len += l2->len;
    solSlist_set_head(l2, NULL);
    solSlist_set_tail(l2, NULL);
    solSlist_set_len(l2, 0);
    solSlist_free(l2);
    return 0;
}

SolSlistNode* solSlistNode_new(void *val)
{
    SolSlistNode *n = sol_calloc(1, sizeof(SolSlistNode));
    if (n == NULL) {
        return NULL;
    }
    solSlistNode_set_val(n, val);
    return n;
}

void solSlistNode_free(SolSlist *l, SolSlistNode *n)
{
    if (solSlistVal_free_func(l)) {
        solSlistVal_free(l, solSlistNode_val(n));
    }
    sol_free(n);
}

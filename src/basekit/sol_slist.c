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
    do {
        nn = solSlistNode_next(n);
        solSlistNode_free(l, n);
        n = nn;
    } while (n);
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
    do {
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
    } while (n);
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
    do {
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
    } while(cn);
    return 1;
}

int solSlist_remove(SolSlist *l, void *v)
{
    SolSlistNode *n = solSlist_head(l);
    SolSlistNode *pn = NULL;
    SolSlistNode *nn;
    do {
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
    } while (n);
    return 1;
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

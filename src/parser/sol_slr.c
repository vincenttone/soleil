#include <string.h>
#include "sol_slr.h"

int solSLRParser_prepare(SolSLRParser *p)
{
    if (p == NULL || p->s == NULL) {
        return -1;
    }
    if (solLRSymbol_is_origin(p->s) == 0
        || solLRSymbol_is_nonterminal(p->s) == 0
        ) {
        return -2;
    }
    if (solSLRParser_compute_items_collection(p, p->s, 1) != 0) {
        return 1;
    }
    return 0;
}

int solSLRParser_compute_items_collection(SolSLRParser *p, SolLRSymbol *s, char k)
{
    if (p == NULL
        || s == NULL
        || s->p == NULL
        || solList_head(s->p) == NULL
        || solList_len(s->p) == 0
        ) {
        return -1;
    }
    SolListNode *n = solList_head(s->p);
    SolLRProduct *cp;
    size_t i = 0;
    size_t j;
    do {
        cp = (SolLRProduct*)(solListNode_val(n));
        j = 0;
        do {
            if (k == 0 && j == 0) { // is not kernel item
                continue;
            }
            if (solSLRParser_register_items_collection(p, cp, j) != 0) {
                return 1;
            }
        } while (++j < solLRProduct_size(cp));
    } while ((n = solListNode_next(n)));
    return 0;
}

int solSLRParser_register_items_collection(SolSLRParser *p, SolLRProduct *p, size_t pos)
{
    size_t state =  solSLRParser_generate_state(p);
    SolLRItemCol *c = solSLRParser_find_items_collection(p, state);
    if (c == NULL) {
        return -1;
    }
    SolLRItem *i = sol_alloc(sizeof(SolLRItem));
    if (i == NULL) {
        return -2;
    }
    i->p = p;
    i->pos = pos;
    SolLRSymbol *s = solLRProduct_find_symbol(p, pos);
    if (solLRSymbol_is_nonterminal(s)) {
        if (solSLRParser_compute_items_collection(p, s, 0) != 0) {
            return -3;
        }
    }
    return 0;
}

SolLRItemCol* solSLRParser_find_items_collection(SolSLRParser *p, size_t s)
{
    if (s > p->ics) {
        SolLRItemCol *ic = p->_ic;
        p->_ic = sol_calloc(p->ics * 2, sizeof(SolLRItemCol));
        if (p->_ic == NULL) {
            p->_ic = ic;
            return NULL;
        }
        memcpy(p->_ic, ic, p->ics);
        p->ics = p->ics * 2;
        sol_free(ic);
    }
    return p->_ic + s;
}

int solSLRParser_compute_follow(SolSLRParser *p)
{
    if (p == NULL || p->p == NULL) {
        return -1;
    }
    if (solList_len(p->p) == 0) {
        return -2;
    }
    SolLRProduct *c = solList_head(p->p);
}

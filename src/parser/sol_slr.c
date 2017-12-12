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
    if (solSLRParser_compute_items_collection(p, p->s) != 0) {
        return 1;
    }
    if (solSLRParser_compute_goto(p) != 0) {
        return 1;
    }
    return 0;
}

SolLRItemCol* solSLRParser_generate_items_collection()
{
    size_t state = solSLRParser_generate_state(p);
    SolLRItemCol *c = solSLRParser_find_items_collection(p, state);
    if (c == NULL) {
        return NULL;
    }
    c->s = state;
    return c;
}

int solSLRParser_compute_items_collection(SolSLRParser *p, SolLRSymbol *s)
{
    if (p == NULL || s == NULL || s->p == NULL) {
        return -1;
    }
    if (solList_head(s->p) == NULL || solList_len(s->p) == 0) {
        return -2;
    }
    SolListNode *n = solList_head(s->p);
    SolLRProduct *product = (SolLRProduct*)(solListNode_val(n));
    size_t i = 0;
    size_t j = 0;
    SolLRItemCol *c;
    SolLRItem *item;
    do {
        c = solSLRParser_generate_items_collection();
        // kernel items
        item = sol_alloc(sizeof(SolLRItem));
        if (item == NULL) {
            return -2;
        }
        item->p = product;
        item->pos = j;
        if (solList_add(c->is, i) == NULL) {
            return -2;
        }
        // nonkernel items
        s = solLRProduct_find_symbol(item->p, item->pos);
        if (solLRSymbol_is_nonterminal(s)) {
            solSLRParser_process_nonkernel_items(p, c, s);
            while ((s = solStack_pop(p->_stk))) {
                s->ext = NULL;
            }
        }
    } while (++j < solLRProduct_size(product));
    return 0;
}

int solSLRParser_process_nonkernel_items(SolSLRParser *p, SolLRItemCol *c, SolLRSymbol *s)
{
    SolListNode *n = solList_head(s->p);
    SolLRProduct *product;
    SolLRSymbol *sym;
    do {
        product = (SolLRProduct*)(solListNode_val(n));
        sym = solLRProduct_right(product);
        if (solLRSymbol_is_nonterminal(s) && s->ext == NULL) {
            s->ext = p;
            solStack_push(p->_stk, s);
            solSLRParser_process_nonkernel_items(p, c, sym);
        }
    } while ((n = solListNode_next(n)));
}

int solSLRParser_checking_items(SolSLRParser *p, SolList *src)
{
    SolListNode *n1 = solList_head(src);
    SolListNode *n2;
    SolLRItem i;
    SolLRSymbol *s;
    SolLRProduct *product;
    do {
        i = (SolLRItem*)(solListNode_val(n1));
        s = solLRProduct_find_symbol(i->p, i->pos);
        if (solLRSymbol_is_nonterminal(s)) {
        }
        n = solList_head(s->p);
        do {
            product = (SolLRProduct*)(solListNode_val(n));
            s1 = solLRProduct_right(product);
            if (solLRSymbol_is_nonterminal(s1)) {
                SolList *l = solList_new();
                solSLRParseer_add_checking_items(p, s1, l);
            }
        } while ((n2 = solListNode_next(s->p)));
    } while ((n1 = solListNode_next(n1)));
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

int solSLRParser_compute_goto(SolSLRParser *p)
{
    size_t i = 0;
    SolLRItemCol *c;
    do {
        c = (SolLRItemCol*)(p->_ic + i);
        
    } while (++i < p->ics);
    return 0;
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

#include <string.h>
#include "sol_slr.h"
#include "sol_hash.h"

/**
 * create a new slr parser
 * @return SolSLRParser *
 */
SolSLRParser* solSLRParser_new()
{
    SolSLRParser *p = sol_alloc(sizeof(SolSLRParser));
    if (p == NULL) {
        return NULL;
    }
    p->ig = 0;
    p->ics = SolSLRParserItemCol_INIT_SIZE;
    p->_stk = solStack_new();
    p->_ss = solRBTree_new();
    p->_ic = sol_calloc(SolSLRParserItemCol_INIT_SIZE, sizeof(SolLRItemCol));
    p->s = solSymbol_new(p, SolLRSymbolFlag_ORIGIN);
    return p;
}
/**
 * free paraser
 * @params SolSLRParser *
 * @return void
 */
void solSLRParser_free(SolSLRParser *p)
{
    if (p->_stk) {
        solStack_free(p->_stk);
    }
    solRBTree_free(p->_ss);
    size_t i;
    SolLRItemCol *c;
    for (i = 0; i < p->ics; i++) {
        c = solSLRParser_find_items_collection(p, i);
        if (c) {
            solLRItemCol_free(c);
        }
    }
    sol_free(p->_ic);
    sol_free(p);
}

SolLRItem* solLRItem_new(SolLRProduct *product, size_t pos, char is_kernel)
{
    SolLRItem *i = sol_alloc(sizeof(SolLRItem));
    if (i == NULL) {
        return NULL;
    }
    i->p = product;
    i->pos = pos;
    i->k = is_kernel;
    return i;
}

void solLRItem_free(SolLRItem *item)
{
    sol_free(item);
}

SolLRProduct* solLRProduct_new(size_t len, SolLRSymbol *s, ...)
{
    if (s == NULL || solLRSymbol_is_terminal(s)) {
        return NULL;
    }
    SolLRProduct *p = sol_alloc(sizeof(SolLRProduct));
    p->s = s;
    p->len = len;
    p->r = sol_alloc(sizeof(SolLRSymbol) * len);
    SolLRSymbol *sym;
    size_t i;
    va_list al;
    va_start(al, s);
    for (i = 0; i < len; i++) {
        s = va_arg(al, SolLRProduct*);
        sym = solLRProduct_find_symbol(p, i);
        *sym = *s;
    }
    va_end(al);
    solList_add(s->p, p);
    return p;
}

void solLRProduct_free(SolLRProduct *p)
{
    sol_free(p->r);
    sol_free(p);
}

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
    SolLRProduct *product = (SolLRProduct*)(solListNode_val(solList_head(p->s->p)));
    SolLRItem *i = solLRItem_new(product, 0, SolLRItem_KERNEL);
    SolLRItemCol *c = solSLRParser_generate_items_collection();
    if (solList_add(c->items, i) == NULL) {
        return -3;
    }
    if (solSLRParser_compute_items_collections(p, c) != 0) {
        return 1;
    }
    if (solSLRParser_compute_goto(p) != 0) {
        return 2;
    }
    if (solSLRParser_compute_follow(p) != 0) {
        return 3;
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

int solSLRParser_compute_items_collections(SolSLRParser *p, SolLRItemCol *c)
{
    if (p == NULL || c == NULL || c->is == NULL) {
        return -1;
    }
    if (solList_head(c->items) == NULL || solList_len(c->items) == 0) {
        return -2;
    }
    SolListNode *n = solList_head(c->items);
    SolLRItemCol *col;
    SolLRItem *item;
    SolLRSymbol *s;
    do {
        item = (SolLRItem*)(solListNode_val(n));
        // nonkernel items
        s = solLRProduct_find_symbol(item->p, item->pos);
        col = solRBTree_search(c->nc, s);
        if (col == NULL) {
            col = solSLRParser_generate_items_collection();
            col->sym = s;
            solRBTree_insert(c->nc, col);
        }
        solList_add(col->items, item);
        if (solLRSymbol_is_nonterminal(s)) {
            solSLRParser_compute_nonkernel_items(p, c, s);
        }
    } while ((n = solListNode_next(n)));
    SolRBTreeIter* rbti = solRBTreeIter_new(n->nc, solRBTree_root(n->nc), SolRBTreeIterTT_inorder);
    SolLRItem *item1;
    do { // compute next kernel items collections
        col = (SolLRItemCol*)(solRBTreeIter_current_val(rbti));
        n = solList_head(col->items);
        do {
            item = (SolLRItem*)(solListNode_val(n));
            if (item->pos > item->p->len) {
                continue;
            }
            item1 = sol_alloc(sizeof(SolLRItem));
            if (item1 == NULL) {
                return -2;
            }
            item1->p = item->p;
            item1->pos = (item->pos) + 1;
            if (solList_add(col->items, item1) == NULL) {
                return -3;
            }
            if (item->k == SolLRItem_NONKERNEL) {
                solLRItem_free(item);
                solListNode_set_val(n, NULL);
            }
        } while ((n = solListNode_next(n)));
        solList_remove(col->items, NULL); // clean up
        if (solSLRParser_compute_items_collections(p, col) != 0) {
            return 1;
        }
    } while (solRBTreeIter_next(rbti) != NULL);
    return 0;
}

int solSLRParser_compute_nonkernel_items(SolSLRParser *p, SolLRItemCol *c, SolLRSymbol *s)
{
    SolListNode *n = solList_head(s->p);
    SolLRProduct *product;
    SolLRSymbol *sym;
    SolLRItem *item;
    SolLRItemCol *col;
    SolList *l;
    do {
        product = (SolLRProduct*)(solListNode_val(n));
        sym = solLRProduct_right(product);
        item = solLRItem_new(product, 0, SolLRItem_NONKERNEL);
        col = solRBTree_search(c->nc, sym);
        if (col == NULL) {
            col = solSLRParser_generate_items_collection();
            col->sym = s;
            solRBTree_insert(c->nc, col);
        }
        solList_add(col->items, item);
        if (solLRSymbol_is_nonterminal(sym) && solLRSymbol_is_free(sym)) {
            solLRSymbol_set_computing(sym)
            solStack_push(p->_stk, sym);
        }
    } while ((n = solListNode_next(n)));
    while ((sym = solStack_pop(p->_stk))) {
        solSLRParser_process_nonkernel_items(p, c, sym);
        solLRSymbol_set_free(sym)
    }
    return 0;
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

int solSLRParser_compute_goto(SolSLRParser *p)
{
    if (p == NULL || p->_ic == NULL) {
        return -1;
    }
    size_t i;
    SolLRItemCol *c1;
    SolLRItemCol *c2;
    SolRBTreeIter* rbti;
    for (i = 0; i < p->ics; i++) {
        c1 = p->_ic + i;
        rbti = solRBTreeIter_new(c1->nc, solRBTree_root(c1->nc), SolRBTreeIterTT_inorder);
        do {
            c2 = (SolLRItemCol*)(solRBTreeIter_current_val(n));
            if (solSLRParser_add_to_goto(p, c1->s, c2->sym, c2->s) != 0) {
                return 1;
            }
        } while (solRBTreeIter_next(rbti));
    }
    return 0;
}

int solSLRParser_add_to_goto(SolSLRParser *p, size_t s1, SolLRSymbol *symbol, size_t s2)
{
    
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

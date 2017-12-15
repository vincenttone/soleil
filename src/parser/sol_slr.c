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
    p->gen = 0;
    p->size_cols = SolSLRParserItemCol_INIT_SIZE;
    p->stk = solStack_new();
    p->symbols = solRBTree_new();
    p->collections = sol_calloc(SolSLRParserItemCol_INIT_SIZE, sizeof(SolLRItemCol));
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
    if (p->stk) {
        solStack_free(p->stk);
    }
    solRBTree_free(p->symbols);
    size_t i;
    SolLRItemCol *c;
    for (i = 0; i < p->size_cols; i++) {
        c = solSLRParser_find_items_collection(p, i);
        if (c) {
            solLRItemCol_free(c);
        }
    }
    sol_free(p->collections);
    sol_free(p);
}

SolLRItem* solLRItem_new(SolLRProduct *product, size_t pos)
{
    SolLRItem *i = sol_alloc(sizeof(SolLRItem));
    if (i == NULL) {
        return NULL;
    }
    i->p = product;
    i->pos = pos;
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
    SolLRItem *i = solLRItem_new(product, 0);
    SolLRItemCol *c = solSLRParser_generate_items_collection();
    if (solList_add(c->items, i) == NULL) {
        return -3;
    }
    if (solSLRParser_compute_items_collections(p, c) != 0) {
        return 1;
    }
    if (solSLRParser_compute_parsing_table(p) != 0) {
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
    c->state = state;
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
    if ((c->flag & SolLRItemCol_FLAG_END) > 0) {
        return 0;
    }
    SolListNode *n = solList_head(c->items);
    SolLRItemCol *col;
    SolLRItem *item;
    SolLRSymbol *s;
    do {
        item = (SolLRItem*)(solListNode_val(n));
        // nonkernel items
        if (item->pos > item->p->len) {
            continue;
        }
        if (item->pos == item->p->len) { // the end of product
            col = solSLRParser_generate_items_collection();
            col->flag |= SolLRItemCol_FLAG_END;
            col->sym = item->p->s; // record product nonterminal symbol when reach end
            solRBTree_insert(c->nc, col);
        } else {
            s = solLRProduct_find_symbol(item->p, item->pos);
            col = solRBTree_search(c->nc, s);
            if (col == NULL) {
                col = solSLRParser_generate_items_collection();
                col->sym = s;
                solRBTree_insert(c->nc, col);
            }
            item = solLRItem_new(item->p, (item->pos) + 1);
            solList_add(col->items, item);
            if (solLRSymbol_is_nonterminal(s)) { // gererate nonkernel items
                solSLRParser_compute_nonkernel_items(p, c, s);
            }
        }
    } while ((n = solListNode_next(n)));
    SolRBTreeIter* rbti = solRBTreeIter_new(c->nc, solRBTree_root(c->nc), SolRBTreeIterTT_inorder);
    do { // compute next kernel items collections
        col = (SolLRItemCol*)(solRBTreeIter_current_val(rbti));
        // compute next step items collections
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
        col = solRBTree_search(c->nc, sym);
        if (col == NULL) {
            col = solSLRParser_generate_items_collection();
            col->sym = s;
            solRBTree_insert(c->nc, col);
        }
        item = solLRItem_new(product, 1);
        solList_add(col->items, item);
        if (solLRSymbol_is_nonterminal(sym) && solLRSymbol_is_idle(sym)) {
            solLRSymbol_set_busy(sym)
            solStack_push(p->stk, sym);
        }
    } while ((n = solListNode_next(n)));
    while ((sym = solStack_pop(p->stk))) {
        solSLRParser_process_nonkernel_items(p, c, sym);
        solLRSymbol_set_idle(sym)
    }
    return 0;
}

int solSLRParser_compute_parsing_table(SolSLRParser *p)
{
    if (p == NULL || p->collections == NULL) {
        return -1;
    }
    size_t i;
    SolLRItemCol *c1;
    SolLRItemCol *c2;
    SolRBTreeIter* rbti;
    for (i = 0; i < p->size_cols; i++) {
        c1 = p->collections + i;
        if (i == 0) { // init state
            p->state = c1->state;
        }
        if ((c1->flag & SolLRItemCol_FLAG_END) > 0) {
            continue;
        }
        rbti = solRBTreeIter_new(c1->nc, solRBTree_root(c1->nc), SolRBTreeIterTT_inorder);
        do {
            c2 = (SolLRItemCol*)(solRBTreeIter_current_val(n));
            if ((c2->flag & SolLRItemCol_FLAG_END) > 0) { // reach the end of product, reduce
                if (solLRSymbol_is_origin(c2->sym)) { // reduce to original, accept
                    if (solSLRParser_record_accept(p, c1->state) != 0) {
                        return 1;
                    }
                } else if (solSLRParser_record_reduce(p, c1->state, c2->sym) != 0) {
                    return 2;
                }
                continue;
            }
            if (solLRSymbol_is_terminal(c2->sym)) { // terminal, shift
                if (solSLRParser_record_shift(p, c1->state, c2->sym, c2->state) != 0) {
                    return 3;
                }
            } else if (solLRSymbol_is_nonterminal(c2->sym)) { // nonerminal, goto
                if (solSLRParser_record_goto(p, c1->state, c2->sym, c2->state) != 0) {
                    return 4;
                }
            } else { // unexpect
                return 5; // error
            }
        } while (solRBTreeIter_next(rbti));
    }
    return 0;
}

int solSLRParser_record_accept(SolSLRParser *p, size_t state)
{
}

int solSLRParser_record_reduce(SolSLRParser *p, size_t state, SolLRSymbol *symbol)
{
    if (solSLRParser_compute_follow(p, symbol) != 0) {
        return 1;
    }
    solSLRParser_follow(p, symbol);
}

int solSLRParser_record_shift(SolSLRParser *p, size_t state1, SolLRSymbol *symbol, size_t state2)
{
}

int solSLRParser_record_goto(SolSLRParser *p, size_t state1, SolLRSymbol *symbol, size_t state2)
{
    
}

int solSLRParser_compute_nullable(SolSLRParser *p, SolSymbol *symbol)
{
    return 0;
}

int solSLRParser_compute_first(SolSLRParser *p, SolSymbol *symbol)
{
    return 0;
}

int solSLRParser_compute_follow(SolSLRParser *p, SolLRSymbol *symbol)
{
    if (p == NULL || p->p == NULL) {
        return -1;
    }
    SolRBTreeIter *iter = solRBTreeIter_new(p->symbols);
    SolLRSymbol *s;
    SolLRProduct *product;
    SolListNode *n;
    size_t i;
    do {
        s = (SolLRSymbol*)(solRBTreeIter_current_val(iter));
        n = solList_head(s->p);
        do {
            product = (SolLRProduct*)(solListNode_val(n));
            for (i = 0; i< product->len; i++) {
                s = (SolLRSymbol*)(product + i);
                if (s == symbol) {
                    if (product->len == (i + 1)) { // compute first(product->s)
                        solSLRParser_compute_first(p, product->s);
                        solSLRParser_add_first_to_follow(p, symbol, product->s);
                    } else {
                        s = (SolLRSymbol*)(product + i + 1);
                        if (solLRSymbol_is_terminal(s)) { // is terminal, record
                            solSLRParser_record_follow(p, s);
                        } else if (solLRSymbol_is_nonterminal(s)) { // nonterminal
                            // compute first(s), add to follow
                            solSLRParser_compute_first(p, s);
                            solSLRParser_merge_first_to_follow(p, symbol, s);
                            solSLRParser_compute_nullable(p, s);
                            if (solLRSymbol_is_nullable(s) == 0) {
                                solSLRParser_compute_follow(p, s);
                                solSLRParser_add_follow_to_follow(p, symbol, s);
                            }
                        }
                    }
                }
            }
        } while ((n = solListNode_next(n)));
    } while ((solRBTreeIter_next(iter)));
    return 0;
}

int solLRSymbol_is_nullable(SolSymbol *symbol)
{
    return 0;
}

int solSLRParser_add_follow_to_follow(SolSLRParser *p, SolSymbol *dest, SolSymbol *src)
{
    return 0;
}

int solSLRParser_add_follow_to_follow(SolSLRParser *p, SolSymbol *dest, SolSymbol *src)
{
    return 0;
}

SolLRItemCol* solSLRParser_find_items_collection(SolSLRParser *p, size_t s)
{
    if (s > p->size_cols) {
        SolLRItemCol *ic = p->collections;
        p->collections = sol_calloc(p->size_cols * 2, sizeof(SolLRItemCol));
        if (p->collections == NULL) {
            p->collections = ic;
            return NULL;
        }
        memcpy(p->collections, ic, p->size_cols);
        p->size_cols = p->size_cols * 2;
        sol_free(ic);
    }
    return p->collections + s;
}
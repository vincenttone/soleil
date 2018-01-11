#include <string.h>
#include <assert.h>
#include "sol_slr.h"
#include "sol_rbtree_iter.h"

/**
 * create a new slr parser
 * @return SolSLRParser
 **/
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
    p->s = solLRSymbol_new(NULL);
    p->s->flag |= SolLRSymbolFlag_ORIGIN;
    p->s = solLRSymbol_new(NULL);
    p->s->flag |= SolLRSymbolFlag_EMPTY;
    p->s->flag |= SolLRSymbolFlag_NULLABLE;
    p->table = solRBTuple_new();
    solRBTuple_set_compare_val_func(p->table, &_solSLRParserField_compare);
    solRBTuple_set_free_val_func(p->table, &_solSLRParserField_free);
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

int _solSLRParserField_compare(void *f1, void *f2)
{
    int flag = ((struct _SolSLRTableField*)f1)->flag & ((struct _SolSLRTableField*)f1)->flag;
    if (flag & SolLRTableFieldFlag_TYPE_STATE) { // state
        if (*(size_t*)(((struct _SolSLRTableField*)f1)->t) > *(size_t*)(((struct _SolSLRTableField*)f2)->t)) {
            return 1;
        } else if (*(size_t*)(((struct _SolSLRTableField*)f1)->t) < *(size_t*)(((struct _SolSLRTableField*)f2)->t)) {
            return -1;
        }
    } else if (flag & SolLRTableFieldFlag_TYPE_SYMBOL) { // symbol
        if ((SolLRSymbol*)(((struct _SolSLRTableField*)f1)->t) > (SolLRSymbol*)(((struct _SolSLRTableField*)f2)->t)) {
            return 1;
        } else if ((SolLRSymbol*)(((struct _SolSLRTableField*)f1)->t) < (SolLRSymbol*)(((struct _SolSLRTableField*)f2)->t)) {
            return -1;
        }
    } else {
        assert("field type is not same");
    }
    return 0;
}

void _solSLRParserField_free(void *field)
{
    sol_free(((struct _SolSLRTableField*)field));
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
    SolLRItemCol *c = solSLRParser_generate_items_collection(p);
    if (solList_add(c->items, i) == NULL) {
        return -3;
    }
    if (solLRItemCol_compute_items_collections(c, &_solSLRParser_generate_items_collection, p) != 0) {
        return 1;
    }
    if (solSLRParser_compute_parsing_table(p) != 0) {
        return 2;
    }
    return 0;
}

SolLRItemCol* _solSLRParser_generate_items_collection(void *p)
{
    return solSLRParser_generate_items_collection((SolSLRParser*)p);
}

SolLRItemCol* solSLRParser_generate_items_collection(SolSLRParser *p)
{
    size_t state = solSLRParser_generate_state(p);
    SolLRItemCol *c = solSLRParser_find_items_collection(p, state);
    if (c == NULL) {
        return NULL;
    }
    c->state = state;
    return c;
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
            c2 = (SolLRItemCol*)(solRBTreeIter_current_val(rbti));
            if ((c2->flag & SolLRItemCol_FLAG_END) > 0) { // reach the end of product, reduce
                if (solLRSymbol_is_origin(c2->sym)) { // reduce to original, accept
                    if (solSLRParser_record_accept(p, c1) != 0) {
                        return 1;
                    }
                } else if (solSLRParser_record_reduce(p, c1, c2->sym) != 0) {
                    return 2;
                }
                continue;
            }
            if (solLRSymbol_is_terminal(c2->sym)) { // terminal, shift
                if (solSLRParser_record_shift(p, c1, c2->sym, c2) != 0) {
                    return 3;
                }
            } else if (solLRSymbol_is_nonterminal(c2->sym)) { // nonerminal, goto
                if (solSLRParser_record_goto(p, c1, c2->sym, c2) != 0) {
                    return 4;
                }
            } else { // unexpect
                return 5; // error
            }
        } while (solRBTreeIter_next(rbti));
    }
    return 0;
}

int solSLRParser_record_accept(SolSLRParser *p, SolLRItemCol *c)
{
    if (p == NULL) {
        return -1;
    }
    struct _SolSLRTableField *s = sol_calloc(1, sizeof(struct _SolSLRTableField));
    s->t = c;
    s->flag |= SolLRTableFieldFlag_TYPE_STATE;
    struct _SolSLRTableField *sym = sol_calloc(1, sizeof(struct _SolSLRTableField));
    sym->t = p->s;
    sym->flag |= SolLRTableFieldFlag_TYPE_SYMBOL;
    if (solRBTuple_put(p->table, 2, s, sym) != 0) {
        return 1;
    }
    return 0;
}

int solSLRParser_record_reduce(SolSLRParser *p, SolLRItemCol *c, SolLRSymbol *symbol)
{
    if (p == NULL || symbol == NULL) {
        return -1;
    }
    if (solLRSymbol_compute_follow(symbol, p->symbols, p->e) != 0) {
        return 1;
    }
    struct _SolSLRTableField *s = sol_calloc(1, sizeof(struct _SolSLRTableField));
    s->t = c;
    s->flag |= SolLRTableFieldFlag_TYPE_STATE;
    struct _SolSLRTableField *sym1 = sol_calloc(1, sizeof(struct _SolSLRTableField));
    sym1->t = symbol;
    sym1->flag |= SolLRTableFieldFlag_TYPE_SYMBOL;
    sym1->flag |= SolLRTableFieldFlag_ACTION_REDUCE;
    struct _SolSLRTableField *sym2;
    SolRBTreeIter *i = solRBTreeIter_new(symbol->follows, solRBTree_root(symbol->follows), SolRBTreeIterTT_inorder);
    do {
        symbol = (SolLRSymbol*)(solRBTreeIter_current_val(i));
        sym2 = sol_calloc(1, sizeof(struct _SolSLRTableField));
        sym2->t = symbol;
        sym2->flag |= SolLRTableFieldFlag_TYPE_SYMBOL;
        if (solRBTuple_put(p->table, 3, s, sym2, sym1) != 0) {
            return 1;
        }
    } while (solRBTreeIter_next(i));
    return 0;
}

int solSLRParser_record_shift(SolSLRParser *p, SolLRItemCol *c1, SolLRSymbol *symbol, SolLRItemCol *c2)
{
    if (p == NULL || symbol == NULL) {
        return -1;
    }
    struct _SolSLRTableField *s1 = sol_calloc(1, sizeof(struct _SolSLRTableField));
    s1->t = c1;
    s1->flag |= SolLRTableFieldFlag_TYPE_STATE;
    struct _SolSLRTableField *sym = sol_calloc(1, sizeof(struct _SolSLRTableField));
    sym->t = symbol;
    sym->flag |= SolLRTableFieldFlag_TYPE_SYMBOL;
    struct _SolSLRTableField *s2 = sol_calloc(1, sizeof(struct _SolSLRTableField));
    s2->t = c2;
    s2->flag |= SolLRTableFieldFlag_TYPE_STATE;
    s2->flag |= SolLRTableFieldFlag_ACTION_SHIFT;
    if (solRBTuple_put(p->table, 3, s1, sym, s2) != 0) {
        return 1;
    }
    return 0;
}

int solSLRParser_record_goto(SolSLRParser *p, SolLRItemCol *c1, SolLRSymbol *symbol, SolLRItemCol *c2)
{
    if (p == NULL || symbol == NULL) {
        return -1;
    }
    struct _SolSLRTableField *s1 = sol_calloc(1, sizeof(struct _SolSLRTableField));
    s1->t = c1;
    s1->flag |= SolLRTableFieldFlag_TYPE_STATE;
    struct _SolSLRTableField *sym = sol_calloc(1, sizeof(struct _SolSLRTableField));
    sym->t = symbol;
    sym->flag |= SolLRTableFieldFlag_TYPE_SYMBOL;
    struct _SolSLRTableField *s2 = sol_calloc(1, sizeof(struct _SolSLRTableField));
    s2->t = c2;
    s2->flag |= SolLRTableFieldFlag_TYPE_STATE;
    s2->flag |= SolLRTableFieldFlag_ACTION_GOTO;
    if (solRBTuple_put(p->table, 3, s1, sym, s2) != 0) {
        return 1;
    }
    return 0;
}

SolLRItemCol* solSLRParser_find_items_collection(SolSLRParser *p, size_t s)
{
	if (p == NULL) {
		return NULL;
	}
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

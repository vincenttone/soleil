#include <assert.h>
#include "sol_slr.h"
#include "sol_rbtree_iter.h"

/**
 * create a new slr parser
 * @return SolSLRParser
 **/
SolSLRParser* solSLRParser_new()
{
    SolSLRParser *p = sol_calloc(1, sizeof(SolSLRParser));
    if (p == NULL) {
        return NULL;
    }
    p->lr = solLRParser_new();
    if (p->lr == NULL) {
        goto oops;
    }
    p->lr->compare_cols = &_solLRItemCols_compare;
    p->lr->compare_symbol_and_col = &_solSLRParser_compare_symbol_and_col;
    p->stk = solStack_new();
    if (p->stk == NULL) {
        goto oops;
    }
    p->symbols = solRBTree_new();
    if (p->symbols == NULL) {
        goto oops;
    }
    solRBTree_set_compare_func(p->symbols, &_solLRItemCols_compare);
    solRBTree_set_val_free_func(p->symbols, &_solLRSymbol_free);
    // start symbol
    p->s = solSLRParser_nonterminal_new(p, NULL);
    if (p->s == NULL) {
        goto oops;
    }
    solLRSymbol_set_flag(p->s, SolLRSymbolFlag_ORIGIN);
    // empty symbol
    p->e = solSLRParser_terminal_new(p, NULL);
    if (p->e == NULL) {
        goto oops;
    }
    solLRSymbol_set_flag(p->e, SolLRSymbolFlag_EMPTY);
    solLRSymbol_set_flag(p->e, SolLRSymbolFlag_NULLABLE);
    p->table = solRBTuple_new();
    if (p->table == NULL) {
        goto oops;
    }
    solRBTuple_set_compare_val_func(p->table, &_solSLRParserField_compare);
    solRBTuple_set_free_val_func(p->table, &_solSLRParserField_free);
    return p;
oops:
    solSLRParser_free(p);
    return NULL;
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
    if (p->symbols) {
        solRBTree_free(p->symbols);
    }
    if (p->lr) {
        solLRParser_free(p->lr);
    }
    if (p->table) {
        solRBTuple_free(p->table);
    }
    if (p) {
        sol_free(p);
    }
}

SolLRSymbol* solSLRParser_terminal_new(SolSLRParser *p, void *v)
{
    SolLRSymbol *s = solLRSymbol_terminal_new(v);
    if (s) {
        if (solRBTree_insert(p->symbols, s) != 0) {
            solLRSymbol_free(s);
            return NULL;
        }
    }
    return s;
}

SolLRSymbol* solSLRParser_nonterminal_new(SolSLRParser *p, void *v)
{
    SolLRSymbol *s = solLRSymbol_nonterminal_new(v);
    if (s) {
        if (solRBTree_insert(p->symbols, s) != 0) {
            solLRSymbol_free(s);
            return NULL;
        }
    }
    return s;
}

int solSLRParser_regiter_symbol(SolSLRParser *p, SolLRSymbol *s)
{
    return solRBTree_insert(p->symbols, s);
}

int solSLRParser_set_begin_product(SolSLRParser *p, SolLRProduct *product)
{
    SolLRProduct *o = solLRProduct_new(p->s, 1, product->s);
    if (o) {
        return 0;
    }
    return 1;
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

int _solSLRParser_compare_symbol_and_col(void *s, void *c)
{
    if ((SolLRSymbol*)s > ((SolLRItemCol*)c)->sym) return 1;
    if ((SolLRSymbol*)s < ((SolLRItemCol*)c)->sym) return -1;
    return 0;
}

int _solLRItemCols_compare(void *s1, void *s2)
{
    if (s1 > s2) return 1;
    if (s2 > s1) return -1;
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
    if ((!solLRSymbol_is_nonterminal(p->s))
        || (!solLRSymbol_is_origin(p->s))
        || p->s->productions == NULL
        ) {
        return -2;
    }
    SolLRProduct *product = (SolLRProduct*)(solListNode_val(solList_head(p->s->productions)));
    SolLRItem *i = solLRItem_new(product, 0);
    SolLRItemCol *c = solLRParser_generate_items_collection(p->lr);
    if (solList_add(c->items, i) == NULL) {
        return -3;
    }
    if (solLRParser_compute_items_collections(p->lr, c) != 0) {
        return 1;
    }
    if (solSLRParser_compute_parsing_table(p) != 0) {
        return 2;
    }
    return 0;
}

int solSLRParser_compute_parsing_table(SolSLRParser *p)
{
    if (p == NULL || p->lr == NULL) {
        return -1;
    }
    SolLRItemCol *c1;
    SolLRItemCol *c2;
    SolRBTreeIter* rbti;
    SolListNode *n = solList_head(p->lr->collections);
    do {
        c1 = (SolLRItemCol*)(solListNode_val(n));
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
    } while ((n = solListNode_next(n)));
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

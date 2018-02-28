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
    p->stk = solStack_new();
    if (p->stk == NULL) {
        goto oops;
    }
    p->symbols = solRBTree_new();
    if (p->symbols == NULL) {
        goto oops;
    }
    p->symbols->ex = p;
    solRBTree_set_compare_func(p->symbols, &_solSLRParser_compare_symbols);
    solRBTree_set_val_free_func(p->symbols, &_solSLRSymbol_free);
    p->table = solRBTuple_new();
    if (p->table == NULL) {
        goto oops;
    }
    p->table->ex = p;
    solRBTuple_set_compare_val_func(p->table, &_solLRParserField_compare);
    // start symbol
    p->lr->origin = solLRSymbol_nonterminal_new(NULL);
    if (p->lr->origin == NULL) {
        goto oops;
    }
    solLRSymbol_set_flag(p->lr->origin, SolLRSymbolFlag_ORIGIN);
    solRBTree_insert(p->symbols, p->lr->origin);
    // empty symbol
    p->lr->empty = solLRSymbol_terminal_new(NULL);
    if (p->lr->empty == NULL) {
        goto oops;
    }
    solLRSymbol_set_flag(p->lr->empty, SolLRSymbolFlag_EMPTY);
    solLRSymbol_set_flag(p->lr->empty, SolLRSymbolFlag_NULLABLE);
    solRBTree_insert(p->symbols, p->lr->empty);
    // end symbol
    p->lr->end = solLRSymbol_terminal_new(NULL);
    if (p->lr->end == NULL) {
        goto oops;
    }
    solLRSymbol_set_flag(p->lr->end, SolLRSymbolFlag_END);
    solRBTree_insert(p->symbols, p->lr->end);
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
    SolLRProduct *o = solLRProduct_new(p->lr->origin, 1, product->s);
    if (o) {
        return 0;
    }
    return 1;
}

int _solSLRParser_compare_symbols(void *s1, void *s2, SolRBTree *tree, int flag)
{
    if (((SolLRSymbol*)s1)->v == NULL) {
        return -1;
    }
    if (((SolLRSymbol*)s2)->v == NULL) {
        return 1;
    }
    SolSLRParser *p = tree->ex;
    return (*p->lr->f_compare_symbol_val)(((SolLRSymbol*)s1)->v, ((SolLRSymbol*)s2)->v);
}

int solSLRParser_prepare(SolSLRParser *p)
{
    if (p == NULL || p->lr->origin == NULL) {
        return -1;
    }
    if ((!solLRSymbol_is_nonterminal(p->lr->origin))
        || (!solLRSymbol_is_origin(p->lr->origin))
        || p->lr->origin->productions == NULL
        ) {
        return -2;
    }
    SolLRProduct *product = (SolLRProduct*)(solListNode_val(solList_head(p->lr->origin->productions)));
    SolLRItem *i = solLRProduct_item(product, 0);
    SolLRItemCol *c = solLRParser_itemCol_new(p->lr, p->lr->origin);
    c = solLRParser_record_items_collection_relations(p->lr, c, NULL);
    if (c == NULL) {
        solLRItemCol_free(c);
    }
    if (solRBTree_insert(c->items, i)) {
        return -3;
    }
    if (solLRParser_compute_items_collections(p->lr, c) != 0) {
        return 1;
    }
    if (0 && solSLRParser_compute_parsing_table(p) != 0) {
        return 2;
    }
    return 0;
}

int solSLRParser_compute_parsing_table(SolSLRParser *p)
{
    if (p == NULL || p->lr == NULL || p->lr->collections == NULL) {
        return -1;
    }
    if (solList_len(p->lr->collections) == 0) {
        return -2;
    }
/*
    SolLRItemCol *c1;
    SolLRItemCol *c2;
    SolRBTreeIter* rbti;
    SolListNode *n = solList_head(p->lr->collections);
    do {
        c1 = (SolLRItemCol*)(solListNode_val(n));
        if ((c1->flag & SolLRItemCol_FLAG_END) > 0) {
            continue;
        }
        if (solRBTree_count(c1->nc) == 0) {
            continue;
        }
        rbti = solRBTreeIter_new(c1->nc, solRBTree_root(c1->nc), SolRBTreeIterTT_preorder);
        do {
            c2 = (SolLRItemCol*)(solRBTreeIter_current_val(rbti));
            
        } while (solRBTreeIter_next(rbti));
        solRBTreeIter_free(rbti);
    } while ((n = solListNode_next(n)));
    */
    return 0;
}

int solSLRParser_record(SolSLRParser *p, SolLRItemCol *c1, SolLRItemCol *c2)
{
    /*
    if (c2->flag & SolLRItemCol_FLAG_END) { // reach the end of product, reduce
        if (solLRSymbol_is_origin(c2->sym)) { // reduce to original, accept
            if (solSLRParser_record_accept(p, c1) != 0) {
                return 1;
            }
        } else if (solSLRParser_record_reduce(p, c1, c2->sym) != 0) {
            return 2;
        }
        goto done;
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
done:
    */
    return 0;
}

int solSLRParser_record_accept(SolSLRParser *p, SolLRTableField *state)
{
    if (p == NULL || state == NULL) {
        return -1;
    }
    SolLRTableField *sym = solLRParserTableField_new(
        p->lr,
        p->lr->end,
        SolLRTableFieldFlag_TYPE_SYMBOL | SolLRTableFieldFlag_ACTION_ACCEPT
        );
    if (solRBTuple_put(p->table, 2, state, sym) != 0) {
        return 1;
    }
    return 0;
}

int solSLRParser_record_reduce(SolSLRParser *p, SolLRTableField *state, SolLRTableField *symbol)
{
    if (p == NULL || symbol == NULL || state == NULL) {
        return -1;
    }
    if (solLRSymbol_compute_follow(((SolLRSymbol*)symbol->target), p->symbols, p->lr->empty, p->lr) != 0) {
        return 1;
    }
    symbol->flag |= SolLRTableFieldFlag_ACTION_REDUCE;
    SolLRTableField *sym;
    SolRBTreeIter *i = solRBTreeIter_new(
        ((SolLRSymbol*)symbol->target)->follows,
        solRBTree_root(((SolLRSymbol*)symbol->target)->follows),
        SolRBTreeIterTT_preorder
        );
    do {
        ;
        sym = solLRParserTableField_new(p->lr, (SolLRSymbol*)(solRBTreeIter_current_val(i)), SolLRTableFieldFlag_TYPE_SYMBOL);
        if (solRBTuple_put(p->table, 3, state, sym, symbol) != 0) {
            return 1;
        }
    } while (solRBTreeIter_next(i));
    solRBTreeIter_free(i);
    return 0;
}

int solSLRParser_record_shift(SolSLRParser *p, SolLRTableField *s1, SolLRTableField *sym, SolLRTableField *s2)
{
    if (p == NULL || sym == NULL || s1 == NULL || s2 == NULL) {
        return -1;
    }
    s2->flag |= SolLRTableFieldFlag_ACTION_SHIFT;
    if (solRBTuple_put(p->table, 3, s1, sym, s2) != 0) {
        return 1;
    }
    return 0;
}

int solSLRParser_record_goto(SolSLRParser *p, SolLRTableField *s1, SolLRTableField *sym, SolLRTableField *s2)
{
    if (p == NULL || sym == NULL || s1 == NULL || s2 == NULL) {
        return -1;
    }
    s2->flag |= SolLRTableFieldFlag_ACTION_GOTO;
    if (solRBTuple_put(p->table, 3, s1, sym, s2) != 0) {
        return 1;
    }
    return 0;
}

void _solSLRSymbol_free(void *symbol)
{
    solLRSymbol_free((SolLRSymbol*)symbol);
}

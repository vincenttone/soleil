#include <assert.h>
#include "sol_slr.h"
#include "sol_rbtree_iter.h"

#ifdef __SOL_DEBUG__
#include <stdio.h>
#endif

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
    p->table->ex = p->lr;
    solRBTuple_set_compare_val_func(p->table, &_solLRParserField_compare);
    // solRBTuple_set_compare_val_func(p->table, &_solSLRParser_compare_fields);
    // stack
    p->stack = solStack_new();
    if (p->stack == NULL) {
        goto oops;
    }
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
    if (p->symbols) {
        solRBTree_free(p->symbols);
    }
    if (p->stack) {
        solStack_free(p->stack);
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

int solSLRParser_read_symbol(SolSLRParser *p, SolLRSymbol *s)
{
    SolLRItemCol *col = solStack_top_val(p->lr->stk);
#ifdef __SOL_DEBUG__
    printf("\nRead symbol: ");
    (*p->lr->f_debug_symbol)(s, p->lr);
    printf("\n- State on stack top: %zu\n", col->state);
#endif
    SolLRTableField *current_field = solLRParserTableField_new(p->lr, col, SolLRTableFieldFlag_TYPE_COL);
    SolLRTableField *field = solLRParserTableField_new(p->lr, s, SolLRTableFieldFlag_TYPE_SYMBOL);
    SolLRTableField *f = solRBTuple_get_first(p->table, 2, current_field, field);
    if (f == NULL) {
        _DEBUG_ALARM_;
        return -1;
    }
#ifdef __SOL_DEBUG__
    printf("Field: ");
    solLRParser_debug_table_field(p->lr, f);
    printf("\n");
#endif
    if (f->flag & SolLRTableFieldFlag_ACTION_SHIFT) {
#ifdef __SOL_DEBUG__
        printf("- Shift -- Push state %zu to stack\n", ((SolLRItemCol*)(f->target))->state);
        printf("@@@ record symbol: ");
        (*p->lr->f_debug_symbol)(s, p->lr);
        printf("\n");
#endif
        solStack_push(p->stack, field);
        solStack_push(p->lr->stk, f->target);
    } else if (f->flag & SolLRTableFieldFlag_ACTION_REDUCE) {
        for (size_t i = 0; i < ((SolLRProduct*)(f->target))->len; i++) {
            col = solStack_pop(p->lr->stk); // col
#ifdef __SOL_DEBUG__
            printf("- Reduce - pop state %zu from stack\n", col->state);
#endif
        }
        col = solStack_top_val(p->lr->stk);
#ifdef __SOL_DEBUG__
        printf("- Reduce - Stack top state %zu\n", col->state);
        printf("@@@ record product: ");
        (*p->lr->f_debug_product)(((SolLRProduct*)(f->target)), p->lr);
        printf("\n");
#endif
        solStack_push(p->stack, f);
        current_field->target = col;
        current_field->flag = SolLRTableFieldFlag_TYPE_COL;
        field = solLRParserTableField_new(
            p->lr,
            ((SolLRProduct*)(f->target))->s,
            SolLRTableFieldFlag_TYPE_SYMBOL
            );
        f = solRBTuple_get_first(p->table, 2, current_field, field); // GOTO
        if (f == NULL) {
            _DEBUG_ALARM_;
            return -2;
        }
#ifdef __SOL_DEBUG__
        printf("- GOTO - push state %zu to stack\n", ((SolLRItemCol*)(f->target))->state);
#endif
        solStack_push(p->lr->stk, f->target);
        return solSLRParser_read_symbol(p, s);
        // output
    } else if (f->flag & SolLRTableFieldFlag_ACTION_ACCEPT) {
        // accept
#ifdef __SOL_DEBUG__
        printf("ACCEPT!!!!\n");
#endif
        return 0;
    } else {
        _DEBUG_ALARM_;
        return -3;
    }
    return 1;
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
    p->origin_state = c;
    if (solStack_push(p->lr->stk, c)) {
        return 2;
    }
    if (solSLRParser_compute_parsing_table(p) != 0) {
        return 3;
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
    if (solRBTuple_list(p->lr->col_rel, &_solSLRParser_record, p)) {
        return 1;
    }
    return 0;
}

int solSLRParser_record(SolSLRParser *p, SolList *l)
{
    SolListNode *n = solList_head(l);
    SolLRTableField *f1 = (SolLRTableField*)(solListNode_val(n));
    SolLRTableField *f2;
    SolLRItemCol *col = f1->target;
    // accept & reduce
    if (col->ends && solList_len(col->ends)) {
        SolListNode *item_node = solList_head(col->ends);
        SolLRItem *item = solListNode_val(item_node);
        do {
            if (solLRSymbol_is_origin(item->p->s)) { // accept
                if (solSLRParser_record_accept(p, f1)) {
                    _DEBUG_ALARM_;
                    return 2;
                }
            } else { // reduce
                if (solSLRParser_record_reduce(p, f1, item->p)) {
                    _DEBUG_ALARM_;
                    return 3;
                }
            }
        } while((item_node = solListNode_next(item_node)));
    }
    if (solList_len(l) == 3) { // shift & goto
        f2 = (SolLRTableField*)(solListNode_val(solListNode_next(n)));
        SolLRSymbol *s = f2->target;
        if (solLRSymbol_is_terminal(s)) { // terminal, shift
            if (solSLRParser_record_shift(p, f1, f2, (SolLRTableField*)(solListNode_val(solList_tail(l))))) {
                _DEBUG_ALARM_;
                return 4;
            }
        } else if (solLRSymbol_is_nonterminal(s)) { // nonerminal, goto
            if (solSLRParser_record_goto(p, f1, f2, (SolLRTableField*)(solListNode_val(solList_tail(l))))) {
                _DEBUG_ALARM_;
                return 5;
            }
        } else {
            _DEBUG_ALARM_;
            return -1;
        }
    }
    return 0;
}

int _solSLRParser_record(SolList *l, SolRBTuple *t, void *d)
{
    return solSLRParser_record((SolSLRParser*)d, l);
}

int solSLRParser_record_accept(SolSLRParser *p, SolLRTableField *state)
{
    if (p == NULL || state == NULL) {
        return -1;
    }
    SolLRTableField *sym = solLRParserTableField_new(
        p->lr,
        p->lr->end,
        SolLRTableFieldFlag_TYPE_SYMBOL
        );
    SolLRTableField *end = solLRParserTableField_new(
        p->lr,
        p->origin_state,
        SolLRTableFieldFlag_TYPE_COL | SolLRTableFieldFlag_ACTION_ACCEPT
        );
    if (solRBTuple_put(p->table, 3, state, sym, end) != 0) {
        return 1;
    }
    return 0;
}

int solSLRParser_record_reduce(SolSLRParser *p, SolLRTableField *state, SolLRProduct *product)
{
    if (p == NULL || product == NULL || state == NULL) {
        return -1;
    }
    if (solLRSymbol_compute_follow(product->s, p->symbols, p->lr->empty, p->lr) != 0) {
        return 1;
    }
    SolLRTableField *field = solLRParserTableField_new(
        p->lr,
        product,
        SolLRTableFieldFlag_ACTION_REDUCE | SolLRTableFieldFlag_TYPE_PRODUCT
        );
    SolLRTableField *end = solLRParserTableField_new(
        p->lr,
        p->lr->end,
        SolLRTableFieldFlag_TYPE_SYMBOL
        );
    SolLRTableField *sym;
    SolRBTreeIter *i = solRBTreeIter_new(
        product->s->follows,
        solRBTree_root(product->s->follows),
        SolRBTreeIterTT_preorder
        );
    do {
        sym = solLRParserTableField_new(p->lr, (SolLRSymbol*)(solRBTreeIter_current_val(i)), SolLRTableFieldFlag_TYPE_SYMBOL);
        if (solRBTuple_put(p->table, 3, state, sym, field) != 0) {
            return 1;
        }
        if (solRBTuple_put(p->table, 3, state, end, field) != 0) {
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
    SolLRTableField *sf = solLRParserTableField_clone(p->lr, s2, SolLRTableFieldFlag_ACTION_SHIFT);
    if (solRBTuple_put(p->table, 3, s1, sym, sf) != 0) {
        return 1;
    }
    return 0;
}

int solSLRParser_record_goto(SolSLRParser *p, SolLRTableField *s1, SolLRTableField *sym, SolLRTableField *s2)
{
    if (p == NULL || sym == NULL || s1 == NULL || s2 == NULL) {
        return -1;
    }
    SolLRTableField *sf = solLRParserTableField_clone(p->lr, s2, SolLRTableFieldFlag_ACTION_GOTO);
    if (solRBTuple_put(p->table, 3, s1, sym, sf) != 0) {
        return 1;
    }
    return 0;
}

void _solSLRSymbol_free(void *symbol)
{
    solLRSymbol_free((SolLRSymbol*)symbol);
}

int _solSLRParser_compare_fields(void *f1, void *f2, SolRBTuple *t, int ext)
{
    return solLRParserField_compare(
        ((SolSLRParser*)(t->ex))->lr,
        (SolLRTableField*)f1,
        (SolLRTableField*)f2
        );
}

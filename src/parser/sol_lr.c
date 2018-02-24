#include <string.h>
#include "sol_lr.h"
#include "sol_stack.h"
#include "sol_rbtree_iter.h"

#ifdef __SOL_DEBUG__
#include <stdio.h>
#endif

SolLRSymbol* solLRSymbol_new(void *symbol)
{
    SolLRSymbol *s = sol_calloc(1, sizeof(SolLRSymbol));
    s->v = symbol;
    return s;
}

SolLRSymbol* solLRSymbol_terminal_new(void *symbol)
{
    SolLRSymbol *s = solLRSymbol_new(symbol);
    if (s) {
        s->flag |= SolLRSymbolFlag_TERMINAL;
    }
    return s;
}

SolLRSymbol* solLRSymbol_nonterminal_new(void *symbol)
{
    SolLRSymbol *s = solLRSymbol_new(symbol);
    if (s) {
        s->flag |= SolLRSymbolFlag_NONTERMINAL;
    }
    return s;
}

void solLRSymbol_free(SolLRSymbol *symbol)
{
    if (symbol->productions) {
        solList_free(symbol->productions);
    }
    if (symbol->firsts) {
        solRBTree_free(symbol->firsts);
    }
    if (symbol->follows) {
        solRBTree_free(symbol->follows);
    }
    sol_free(symbol);
}

void _solLRSymbol_free(void *symbol)
{
    solLRSymbol_free((SolLRSymbol*)symbol);
}

SolLRProduct* solLRProduct_new(SolLRSymbol *s, size_t len, ...)
{
    if (s == NULL || solLRSymbol_is_terminal(s)) {
        return NULL;
    }
    SolLRProduct *p = sol_alloc(sizeof(SolLRProduct));
    if (p == NULL) {
        return NULL;
    }
    p->s = s;
    p->len = len;
    p->r = sol_calloc(len, sizeof(SolLRSymbol*));
    if (p->r == NULL) {
        goto oops;
    }
    p->items = sol_calloc(len + 1, sizeof(SolLRItem));
    if (p->items == NULL) {
        goto oops;
    }
    SolLRSymbol **sym;
    SolLRSymbol *sarg;
    size_t i;
    va_list al;
    va_start(al, len);
    for (i = 0; i < len; i++) {
        sarg = va_arg(al, SolLRSymbol*);
        sym = solLRProduct_find_symbol(p, i);
        *sym = sarg;
    }
    va_end(al);
    if (s->productions == NULL) {
        s->productions = solList_new();
        if (s->productions == NULL) {
            solLRProduct_free(p);
            return NULL;
        }
        solList_set_val_free_func(s->productions, _solLRProduct_free);
    }
    solList_add(s->productions, p);
    return p;
oops:
    solLRProduct_free(p);
    return NULL;
}

void solLRProduct_free(SolLRProduct *p)
{
    if (p->r) {
        sol_free(p->r);
    }
    if (p->items) {
        size_t s;
        SolLRItem *i;
        for (s = 0; s <= p->len; s++) {
            i = (p->items + s);
            if (i->cols) {
                solList_free(i->cols);
            }
        }
        sol_free(p->items);
    }
    sol_free(p);
}

void _solLRProduct_free(void *p)
{
    solLRProduct_free((SolLRProduct*)p);
}

SolLRItem* solLRProduct_item(SolLRProduct *product, size_t pos)
{
    if (pos > product->len) {
        _DEBUG_ALARM_;
        return NULL;
    }
    SolLRItem *i = (product->items + pos);
    if (i->p == NULL) {
        i->p = product;
        i->pos = pos;
        i->cols = solList_new();
        if (pos == product->len) {
            i->flag |= 0x1; // end of product
        }
    }
    return i;
}

SolLRItemCol* solLRParser_itemCol_new(SolLRParser *p, SolLRSymbol *s)
{
    SolLRItemCol *c = solLRItemCol_new(s);
    c->items->ex = p;
    return c;
}

SolLRItemCol* solLRItemCol_new(SolLRSymbol *s)
{
    SolLRItemCol *c = sol_calloc(1, sizeof(SolLRItemCol));
    if (c == NULL) {
        return NULL;
    }
    c->items = solRBTree_new();
    solRBTree_set_compare_func(c->items, &_solLRItem_compare);
    c->sym = s;
    return c;
}

void solLRItemCol_free(SolLRItemCol *c)
{
    if (c->items) {
        solRBTree_free(c->items);
    }
    sol_free(c);
}

void _solLRItemCol_free(void *c)
{
    solLRItemCol_free((SolLRItemCol*)c);
}

int solLRSymbol_compute_nullable(SolLRSymbol *symbol)
{
    if (symbol == NULL || symbol->productions == NULL) {
        return -1;
    }
    if (solLRSymbol_is_terminal(symbol) || solLRSymbol_nullable_computed(symbol)) {
        return 0;
    }
    SolListNode *n = solList_head(symbol->productions);
    SolLRProduct *product;
    SolLRSymbol *s;
    size_t i;
    char nullable;
    do {
        product = solListNode_val(n);
        if (product->len == 0 || product->r == NULL) {
            return -2;
        }
        nullable = 0;
        for (i = 0; i < product->len; i++) {
            s = *(product->r + i);
            if (symbol == s) {
                nullable = 1;
                break;
            }
            if (solLRSymbol_is_nonterminal(s) && !solLRSymbol_nullable_computed(s)) {
                if (solLRSymbol_compute_nullable(s) != 0) {
                    return 1;
                }
            }
            if (!solLRSymbol_is_nullable(s)) {
                nullable = 1;
                break;
            }
        }
        if (nullable == 0) {
            solLRSymbol_set_nullable(s);
            break;
        }
    } while ((n = solListNode_next(n)));
    solLRSymbol_set_nullable_computed(s);
    return 0;
}

int solLRSymbol_compute_first(SolLRSymbol *symbol, SolLRSymbol *empty, SolLRParser *p)
{
    if (symbol == NULL || symbol->productions == NULL) {
        return -1;
    }
    if (solLRSymbol_first_computed(symbol)) {
        return 0;
    }
    if (solLRSymbol_is_nonterminal(symbol)) {
        SolListNode *n = solList_head(symbol->productions);
        SolLRProduct *product;
        SolLRSymbol *s;
        size_t i;
        do {
            product = solListNode_val(n);
            if (product->len == 0 || product->r == NULL) {
                return -2;
            }
            for (i = 0; i < product->len; i++) {
                s = *(product->r + i);
                if (solLRSymbol_is_terminal(s)) {
                    if (solLRSymbol_record_first(symbol, s, p) != 0) {
                        return 1;
                    }
                } else if (solLRSymbol_compute_first(s, empty, p) != 0) {
                    if (s->firsts) {
                        if (solLRSymbol_copy_firsts(s->firsts, symbol) != 0) {
                            return 2;
                        }
                    }
                }
                if (solLRSymbol_compute_nullable(s) != 0) {
                    return 3;
                }
                if (!solLRSymbol_is_nullable(s)) {
                    break;
                }
                if (i + 1 == product->len) {
                    if (solLRSymbol_record_first(symbol, empty, p) != 0) {
                        return 4;
                    }
                }
            }
        } while ((n = solListNode_next(n)));
    }
    solLRSymbol_set_first_computed(symbol);
    return 0;
}

int solLRSymbol_compute_follow(SolLRSymbol *symbol, SolRBTree *symbols, SolLRSymbol *empty, SolLRParser *p)
{
    if (symbols == NULL || symbol->productions == NULL) {
        return -1;
    }
    if (solLRSymbol_follow_computed(symbol)) {
        return 0;
    }
    SolRBTreeIter *iter = solRBTreeIter_new(symbols, solRBTree_root(symbols), SolRBTreeIterTT_inorder);
    SolLRSymbol *s;
    SolLRProduct *product;
    SolListNode *n;
    size_t i;
    do {
        s = (SolLRSymbol*)(solRBTreeIter_current_val(iter));
        if (!solLRSymbol_is_nonterminal(s)) {
            continue;
        }
        n = solList_head(s->productions);
        do {
            product = (SolLRProduct*)(solListNode_val(n));
            for (i = 0; i< product->len; i++) {
                s = *(product->r + i);
                if (s == symbol) {
                    if (product->len == (i + 1)) { // is the last of product
                        // compute product nonterminal's follow
                        if (solLRSymbol_compute_follow(product->s, symbols, empty, p) != 0 ) {
                            return 1;
                        }
                        if (product->s->follows) {
                            if (solLRSymbol_copy_follows(product->s->follows, symbol) != 0) {
                                return 2;
                            }
                        }
                    } else {
                        s = *(product->r + i + 1);
                        if (solLRSymbol_is_terminal(s)) { // is terminal, record
                            if (solLRSymbol_record_follow(symbol, s, p) != 0) {
                                return 3;
                            }
                        } else if (solLRSymbol_is_nonterminal(s)) { // nonterminal
                            // compute s's firsts, add to follow
                            if (solLRSymbol_compute_first(s, empty, p) != 0) {
                                return 1;
                            }
                            // add s's first to symbol's follow
                            if (s->firsts) {
                                if (solLRSymbol_copy_follows(s->firsts, symbol) != 0) {
                                    return 2;
                                }
                            }
                            if (solLRSymbol_compute_nullable(s) != 0) {
                                return 3;
                            }
                            if (solLRSymbol_is_nullable(s) == 0) {
                                if (solLRSymbol_compute_follow(s, symbols, empty, p) != 0) {
                                    return 4;
                                }
                                // add s's follows to symbol's follow
                                if (s->follows) {
                                    if (solLRSymbol_copy_follows(s->follows, symbol) != 0) {
                                        return 2;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } while ((n = solListNode_next(n)));
    } while (solRBTreeIter_next(iter));
    solLRSymbol_set_follow_computed(symbol);
    solRBTreeIter_free(iter);
    return 0;
}

/**
 * @desc record symbol's first
 * @param symbol
 * @param first (of symbol)
 * @param lr parser
 * @return 0 when success, others when failed
 */
int solLRSymbol_record_first(SolLRSymbol *symbol, SolLRSymbol *first, SolLRParser *p)
{
    if (symbol == NULL || first == NULL) {
        return -1;
    }
    if (symbol->firsts == NULL) {
        symbol->firsts = solRBTree_new();
        if (symbol->firsts == NULL) {
            return -2;
        }
        symbol->firsts->ex = p;
        solRBTree_set_compare_func(symbol->firsts, &_solLRParser_compare_symbols);
    }
    if (solRBTree_insert(symbol->firsts, first) != 0) {
        return 1;
    }
    return 0;
}
/**
 * @desc record symbol's follow
 * @param symbol
 * @param follow (of symbol)
 * @param lr parser
 * @return 0 when success, others when failed
 */
int solLRSymbol_record_follow(SolLRSymbol *symbol, SolLRSymbol *follow, SolLRParser *p)
{
    if (symbol == NULL || follow == NULL) {
        return -1;
    }
    if (symbol->follows == NULL) {
        symbol->follows = solRBTree_new();
        if (symbol->follows == NULL) {
            return -2;
        }
        symbol->follows->ex = p;
        solRBTree_set_compare_func(symbol->follows, &_solLRParser_compare_symbols);
    }
    if (solRBTree_insert(symbol->follows, follow) != 0) {
        return 1;
    }
    return 0;
}

int _solLRSymbol_share_firsts(SolRBTree *firsts, SolRBTreeNode *node, void *symbol)
{
    SolLRSymbol *s = (SolLRSymbol*)solRBTreeNode_val(node);
    return solLRSymbol_record_first((SolLRSymbol*)symbol, s, (SolLRParser*)(firsts->ex));
}

int _solLRSymbol_share_follows(SolRBTree *follows, SolRBTreeNode *node, void *symbol)
{
    SolLRSymbol *s = (SolLRSymbol*)solRBTreeNode_val(node);
    return solLRSymbol_record_follow((SolLRSymbol*)symbol, s, (SolLRParser*)(follows->ex));
}

int solLRParser_compute_items_collections(SolLRParser *p, SolLRItemCol *c)
{
    if (c == NULL || c->items == NULL) {
        _DEBUG_ALARM_;
        return -1;
    }
    if (c->flag & SolLRItemCol_FLAG_END) {
        return 0;
    }
    if (c->flag & SolLRItemCol_FLAG_KERNEL_COMPUTED) {
        return 0;
    }
    if (solRBTree_count(c->items) == 0) {
        _DEBUG_ALARM_;
        return -2;
    }
    SolList *cols = solList_new();
    if (solList_add(cols, c) == NULL) {
        _DEBUG_ALARM_;
        solList_free(cols);
        return -3;
    }
    SolListNode *ln;
    SolLRItemCol *col;
#ifdef __SOL_DEBUG__
    printf("==========compute items collection==========\n");
    (*p->f_debug_item_col)(c, p);
#endif
    if (solRBTree_travelsal_inorder(
            c->items,
            solRBTree_root(c->items),
            &_solLRItemsCols_collect_item,
            cols
            )
        ) {
        _DEBUG_ALARM_;
        solList_free(cols);
        return 1;
    }
    if (solList_len(cols) > 1) {
        ln = solList_head(cols);
        while ((ln = solListNode_next(ln))) { // first node is c
            col = solListNode_val(ln);
            col = solLRParser_record_items_collection_relations(p, col, c);
            if (col == NULL) {
                _DEBUG_ALARM_;
                solList_free(cols);
                return 2;
            }
            if (col != (SolLRItemCol*)(solListNode_val(ln))) {
                continue;
            }
            if ((col->flag & SolLRItemCol_FLAG_COMPUTING) == 0) {
                solStack_push(p->stk, col);
                col->flag |= SolLRItemCol_FLAG_COMPUTING;
            }
#ifdef __SOL_DEBUG__
            printf(" record collection of state %zu -> %zu\n", c->state, col->state);
#endif
        }
    }
    solList_free(cols);
    c->flag |= SolLRItemCol_FLAG_KERNEL_COMPUTED;
    while ((col = solStack_pop(p->stk))) {
        if (solLRParser_compute_items_collections(p, col)) {
            _DEBUG_ALARM_;
            solList_free(cols);
            return 3;
        }
        col->flag |= (~SolLRItemCol_FLAG_COMPUTING);
        col->flag |= SolLRItemCol_FLAG_COMPUTED;
    }
    return 0;
}

int _solLRItemsCols_collect_item(SolRBTree *tree, SolRBTreeNode *node, void *list)
{
    return solLRParser_collect_kernel_item(
        (SolLRParser*)tree->ex,
        (SolList*)list,
        (SolLRItem*)solRBTreeNode_val(node)
        );
}

int solLRParser_collect_kernel_item(SolLRParser *p, SolList *cols, SolLRItem *item)
{
    if (item->pos >= item->p->len) {
        return 0;
    }

#ifdef __SOL_DEBUG__
    printf(" compute kernel item: ");
    (*p->f_debug_item)(item, p);
#endif
    SolLRSymbol *s = *(solLRProduct_find_symbol(item->p, item->pos));
#ifdef __SOL_DEBUG__
    printf(" computing symbol: ");
    (*p->f_debug_symbol)(s, p);
    printf("\n");
#endif
    SolLRItemCol *col = NULL;
    SolListNode *ln = solList_head(cols);
    SolLRItemCol *c = solListNode_val(ln); // first node is pre col
    if (solList_len(cols) > 1) {
         while ((ln = solListNode_next(ln))) {
             if (((SolLRItemCol*)(solListNode_val(ln)))->sym == s) {
                 col = solListNode_val(ln);
                 break;
             }
         }
    }
    if (col == NULL) {
        col = solLRParser_itemCol_new(p, s);
        if (col == NULL) {
            _DEBUG_ALARM_;
            goto oops;
        }
        if (solList_add(cols, col) == NULL) {
            _DEBUG_ALARM_;
            goto oops;
        }
    }
    item = solLRProduct_item(item->p, (item->pos) + 1);
    item->flag |= 0x4;
    solRBTree_insert(col->items, item);
    if (solLRSymbol_is_nonterminal(col->sym)) { // gererate nonkernel items
        if (solLRParser_compute_nonkernel_items(p, c, col)) {
            _DEBUG_ALARM_;
            goto oops;
        }
    }
    return 0;
oops:
    return 1;
}

int solLRParser_compute_nonkernel_items(SolLRParser *p, SolLRItemCol *pre_col, SolLRItemCol *current_col)
{
#ifdef __SOL_DEBUG__
    printf(" computing nonkernel of item col: ");
    (*p->f_debug_item_col)(current_col, p);
    printf("\n");
#endif
    SolLRSymbol *s = current_col->sym;
    SolListNode *n = solList_head(s->productions);
    SolLRProduct *product;
    SolLRSymbol *sym;
    SolLRItem *item;
    SolLRItemCol *col;
    SolList *cols = solList_new();
    SolListNode *ln;
    do {
        product = (SolLRProduct*)(solListNode_val(n));
        sym = *(product->r);
#ifdef __SOL_DEBUG__
        printf("  target product: ");
        (*p->f_debug_product)(product, p);
        printf("  target symbol: ");
        (*p->f_debug_symbol)(sym, p);
        printf("\n");
#endif
        col = NULL;
        if (solList_len(cols)) {
            ln = solList_head(cols);
            do {
                if (((SolLRItemCol*)(solListNode_val(ln)))->sym == sym) {
                    col = solListNode_val(ln);
                    break;
                }
            } while ((ln = solListNode_next(ln)));
        }
        if (col == NULL) {
            col = solLRParser_itemCol_new(p, sym);
            if (col == NULL) {
                _DEBUG_ALARM_;
                goto oops;
            }
            if (solList_add(cols, col) == NULL) {
                _DEBUG_ALARM_;
                goto oops;
            }
        }
        item = solLRProduct_item(product, 1);
        item->flag |= 0x8;
        if (solRBTree_insert(col->items, item)) {
            _DEBUG_ALARM_;
            goto oops;
        }
        continue;
    oops:
        return 1;
    } while ((n = solListNode_next(n)));
    if (solList_len(cols)) {
        ln = solList_head(cols);
        do {
            col = solListNode_val(ln);
            col = solLRParser_record_items_collection_relations(p, col, pre_col);
            if (col == NULL) {
                _DEBUG_ALARM_;
                solList_free(cols);
                return 2;
            }
            if (col != (SolLRItemCol*)(solListNode_val(ln))) {
                continue;
            }
            if (solLRSymbol_is_nonterminal(col->sym) && col->sym != s) {
                if (solLRParser_compute_nonkernel_items(p, pre_col, col)) {
                    _DEBUG_ALARM_;
                    solList_free(cols);
                    return 2;
                }
            }
            if ((col->flag & SolLRItemCol_FLAG_COMPUTING) == 0) {
                solStack_push(p->stk, col);
            }
        } while ((ln = solListNode_next(ln)));
    }
    solList_free(cols);
    return 0;
}

SolLRParser* solLRParser_new()
{
    SolLRParser *p = sol_calloc(1, sizeof(SolLRParser));
    p->gen = 0;
    p->stk = solStack_new();
    if (p->stk == NULL) {
        goto oops;
    }
    p->collections = solList_new();
    if (p->collections == NULL) {
        goto oops;
    }
    solList_set_val_free_func(p->collections, &_solLRItemCol_free);
    p->col_rel = solRBTuple_new();
    if (p->col_rel == NULL) {
        goto oops;
    }
    solRBTuple_set_compare_val_func(p->col_rel, &_solLRParserField_compare);
    p->col_rel->ex = p;
    p->fields = solList_new();
    if (p->fields == NULL) {
        goto oops;
    }
    return p;
oops:
    solLRParser_free(p);
    return NULL;
}

void solLRParser_free(SolLRParser *p)
{
    if (p == NULL) {
        return;
    }
    if (p->stk) {
        solStack_free(p->stk);
    }
    if (p->collections) {
        solList_free(p->collections);
    }
    if (p->fields) {
        if (solList_len(p->fields)) {
            SolListNode *n = solList_head(p->fields);
            do {
                solLRParserTableField_free((SolLRTableField*)(solListNode_val(n)));
            } while ((n = solListNode_next(n)));
        }
        solList_free(p->fields);
    }
    if (p->col_rel) {
        solRBTuple_free(p->col_rel);
    }
    sol_free(p);
}

SolLRItemCol* solLRParser_record_items_collection_relations(SolLRParser *p, SolLRItemCol *col, SolLRItemCol *pre_col)
{
    if (p == NULL) {
        return NULL;
    }
    SolLRTableField *f3;
    if (pre_col == NULL) {
        col->state = ++p->gen;
        f3 = solLRParserTableField_new(p, col, SolLRTableFieldFlag_TYPE_COL);
        goto record;
    }
#ifdef __SOL_DEBUG__
    printf("try record relations of cols:\n");
    (*p->f_debug_item_col)(col, p);
    printf("\n");
    (*p->f_debug_item_col)(pre_col, p);
    printf("\n");
#endif
    SolLRTableField *f1 = solLRParserTableField_new(p, pre_col, SolLRTableFieldFlag_TYPE_COL);
    SolLRTableField *f2 = solLRParserTableField_new(p, col->sym, SolLRTableFieldFlag_TYPE_SYMBOL);
    SolLRItem *item = solRBTreeNode_val(solRBTree_root(col->items));
    if (solList_len(item->cols)) {
        SolListNode *n = solList_head(item->cols);
        SolLRItemCol *c;
        do {
            c = solListNode_val(n);
            if (c == col) continue;
#ifdef __SOL_DEBUG__
            printf(" ---------------------------------\n");
            printf(" compare cols:\n");
            (*p->f_debug_item_col)(col, p);
            printf("\n");
            (*p->f_debug_item_col)(c, p);
            printf("\n");
            printf(" ---------------------------------\n");
#endif
            if (solRBTree_compare_tree(
                    col->items,
                    solRBTree_root(col->items),
                    c->items,
                    solRBTree_root(c->items)
                    ) == 0
                ) {
                f3 = solLRParserTableField_new(p, c, SolLRTableFieldFlag_TYPE_COL);
                if (solRBTuple_put(p->col_rel, 3, f1, f2, f3)) {
                    return NULL;
                }
#ifdef __SOL_DEBUG__
                printf(" -> record of col:");
                (*p->f_debug_item_col)(c, p);
                printf("\n");
                printf(" -> col:");
                (*p->f_debug_item_col)(col, p);
                printf("\n");
#endif
                //solList_remove(item->cols, col);
                solLRItemCol_free(col);
                return c;
            }
        } while ((n = solListNode_next(n)));
    }
#ifdef __SOL_DEBUG__
    else {
        printf("no cols for item\n");
    }
#endif
    col->state = ++p->gen;
    f3 = solLRParserTableField_new(p, col, SolLRTableFieldFlag_TYPE_COL);
    if (f3 == NULL) {
        goto oops;
    }
    if (solRBTuple_put(p->col_rel, 3, f1, f2, f3)) {
        goto oops;
    }
record:
#ifdef __SOL_DEBUG__
    printf(" -> record new col:");
    (*p->f_debug_item_col)(col, p);
#endif
    if (solRBTuple_put(p->col_rel, 1, f3)) {
        goto oops;
    }
    solRBTree_travelsal_inorder(col->items, solRBTree_root(col->items), &_solLRItem_register_collection, col);
    return col;
oops:
    return NULL;
}

int _solLRItem_register_collection(SolRBTree *tree, SolRBTreeNode *node, void *col)
{
    if (((SolLRItem*)solRBTreeNode_val(node))->cols) {
        solList_add(((SolLRItem*)solRBTreeNode_val(node))->cols, (SolLRItemCol*)col);
    }
    return 0;
}

int solLRParser_compare_symbol(SolLRParser *p, SolLRSymbol *s1, SolLRSymbol *s2)
{
    if (s1 == p->origin) {
        return 1;
    } else if (s2 == p->origin) {
        return -1;
    }
    if (s1 == p->empty || s1 == p->end) {
        return -1;
    } else if (s2 == p->empty || s2 == p->end) {
        return 1;
    }
    // tree->ex is SolLRParser
    return (*p->f_compare_symbol_val)(s1->v, s2->v);
}

int solLRItem_compare(SolLRItem *i1, SolLRItem *i2)
{
    if (i1->p == i2->p) {
        if (i1->pos > i2->pos) {
            return 1;
        } else if (i1->pos < i2->pos) {
            return -1;
        }
        return 0;
    } else if ((i1->p > i2->p)) {
        return 1;
    }
    return -1;
}

int _solLRItem_compare(void *i1, void *i2, SolRBTree *t, int flag)
{
    if (i1 == i2) return 0;
    return solLRItem_compare(i1, i2);
}

int _solLRParser_compare_symbols(void *s1, void *s2, SolRBTree *tree, int flag)
{
    if (((SolLRSymbol*)s1)->v == NULL) {
        return -1;
    }
    if (((SolLRSymbol*)s2)->v == NULL) {
        return 1;
    }
    SolLRParser *p = tree->ex;
    return (*p->f_compare_symbol_val)(((SolLRSymbol*)s1)->v, ((SolLRSymbol*)s2)->v);
}

int _solLRParserField_compare(void *f1, void *f2, SolRBTuple *t, int ext)
{
    int flag = ((SolLRTableField*)f1)->flag & ((SolLRTableField*)f2)->flag;
    if ((flag & SolLRTableFieldFlag_TYPE_STATE)
        || (flag & SolLRTableFieldFlag_TYPE_COL)
        ) { // state
        SolLRItemCol *c1 = (SolLRItemCol*)(((struct _SolLRTableField*)f1)->target);
        SolLRItemCol *c2 = (SolLRItemCol*)(((struct _SolLRTableField*)f2)->target);
        if (c1 == c2) return 0;
        if (c1->state > c2->state) {
            return 1;
        } else if (c1->state < c2->state) {
            return -1;
        }
        if (c1 > c2) return 1;
        if (c1 < c2) return -1;
        return 0;
    } else if (flag & SolLRTableFieldFlag_TYPE_SYMBOL) { // symbol
        int c = solLRParser_compare_symbol(
            ((SolLRParser*)(t->ex)),
            (SolLRSymbol*)(((SolLRTableField*)f1)->target),
            (SolLRSymbol*)(((SolLRTableField*)f2)->target)
            );
        if (c != 0) {
            return c;
        }
    } else {
        _DEBUG_ALARM_;
    }
    return 0;
}

SolLRTableField* solLRParserTableField_new(SolLRParser *p, void *target, int flag)
{
    SolLRTableField *f = sol_calloc(1, sizeof(SolLRTableField));
    if (solList_add(p->fields, f) == NULL) {
        solLRParserTableField_free(f);
        return NULL;
    }
    f->target = target;
    f->flag |= flag;
    return f;
}

void solLRParserTableField_free(SolLRTableField *f)
{
    sol_free(f);
}

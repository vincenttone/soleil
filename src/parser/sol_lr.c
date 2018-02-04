#include <string.h>
#include "sol_lr.h"
#include "sol_stack.h"
#include "sol_rbtree_iter.h"

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
        if (pos == product->len) {
            i->flag |= 0x1; // end of product
        }
    }
    return i;
}

SolLRItemCol* solLRItemCol_new()
{
    SolLRItemCol *c = sol_calloc(1, sizeof(SolLRItemCol));
    return c;
}

void solLRItemCol_free(SolLRItemCol *c)
{
    if (c->items) {
        solList_free(c->items);
    }
    if (c->nc) {
        solRBTree_free(c->nc);
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
    if (solList_head(c->items) == NULL || solList_len(c->items) == 0) {
        _DEBUG_ALARM_;
        return -2;
    }
    SolListNode *n = solList_head(c->items);
    SolLRItemCol *col;
    SolLRItem *item;
    SolLRItem *item1;
    SolLRSymbol *s;
    SolLRTableField *f1 = solLRParserTableField_new(p, c, SolLRTableFieldFlag_TYPE_COL);
    SolLRTableField *f2;
    SolLRTableField *f3;
    do {
        item = (SolLRItem*)(solListNode_val(n));
        if (item->flag & 0x2) { // processed
            continue;
        }
        if (item->pos > item->p->len) {
            continue;
        }
        if (item->pos == item->p->len) { // the end of product
            // record product nonterminal symbol when reach end
            col = solLRParser_generate_items_collection(p, item->p->s, 0x0);
            col->flag |= SolLRItemCol_FLAG_END;
            f2 = solLRParserTableField_new(p, p->end, SolLRTableFieldFlag_TYPE_SYMBOL);
            f3 = solLRParserTableField_new(p, col, SolLRTableFieldFlag_TYPE_COL);
            if (solRBTuple_put(p->col_rel, 3, f1, f2, f3)) {
                _DEBUG_ALARM_;
                return 1;
            }
        } else {
            s = *(solLRProduct_find_symbol(item->p, item->pos));
            f2 = solLRParserTableField_new(p, s, SolLRTableFieldFlag_TYPE_SYMBOL);
            f3 = solRBTuple_get_first(p->col_rel, 2, f1, f2);
            if (f3 == NULL) {
                col = solLRParser_generate_items_collection(p, s, 0x0);
                f3 = solLRParserTableField_new(p, col, SolLRTableFieldFlag_TYPE_COL);
                if (solRBTuple_put(p->col_rel, 3, f1, f2, f3)) {
                    _DEBUG_ALARM_;
                    return 2;
                }
                if (col->flag & 0x8) continue;
            } else {
                col = f3->target;
            }
            item1 = solLRProduct_item(item->p, (item->pos) + 1);
            item1->flag |= 0x4;
            solList_add(col->items, item1);
            if (solLRSymbol_is_nonterminal(s)) { // gererate nonkernel items
                if (solLRParser_compute_nonkernel_items(p, c, s)) {
                    _DEBUG_ALARM_;
                    return 3;
                }
            }
        }
        item->flag |= 0x2;
    } while ((n = solListNode_next(n)));
    SolRBTupleRecord *r = solRBTuple_get(p->col_rel, 1, c);
    if (r == NULL) {
        _DEBUG_ALARM_;
        return 4;
    }
    if (solRBTuple_record_travelsal(p->col_rel, r, p)) {
        _DEBUG_ALARM_;
        return 5;
    }
    return 0;
}

int _solLRParser_compute_items_collections(void *field, SolRBTuple *t, size_t level, void *p)
{
    if (t == NULL || field == NULL || p == NULL) {
        _DEBUG_ALARM_;
        return -1;
    }
    if ((((SolLRTableField*)field)->flag & SolLRTableFieldFlag_TYPE_COL) == 0) {
        _DEBUG_ALARM_;
        return -2;
    }
    return solLRParser_compute_items_collections((SolLRParser*)p, ((SolLRTableField*)field)->target);
}

int solLRParser_compute_nonkernel_items(SolLRParser *p, SolLRItemCol *c, SolLRSymbol *s)
{
    SolStack *stk = solStack_new();
    SolListNode *n = solList_head(s->productions);
    SolLRProduct *product;
    SolLRSymbol *sym;
    SolLRItem *item;
    SolLRItemCol *col;
    SolLRTableField *f1 = solLRParserTableField_new(p, c, SolLRTableFieldFlag_TYPE_COL);
    SolLRTableField *f2;
    SolLRTableField *f3;
    do {
        product = (SolLRProduct*)(solListNode_val(n));
        sym = *(product->r);
        f2 = solLRParserTableField_new(p, sym, SolLRTableFieldFlag_TYPE_SYMBOL);
        f3 = solRBTuple_get_first(p->col_rel, 2, f1, f2);
        if (f3 == NULL) {
            col = solLRParser_generate_items_collection(p, sym, 0x4);
            f3 = solLRParserTableField_new(p, col, SolLRTableFieldFlag_TYPE_COL);
            f3->flag |= SolLRTableFieldFlag_COL_FROM_NONKERNEL;
            if (solRBTuple_put(p->col_rel, 3, f1, f2, f3)) {
                solStack_free(stk);
                return 1;
            }
        } else {
            col = f3->target;
        }
        item = solLRProduct_item(product, 1);
        item->flag |= 0x8;
        if (solList_add(col->items, item) == NULL) {
            return 3;
        }
        if (solLRSymbol_is_nonterminal(sym) && solLRSymbol_is_idle(sym)) {
            solLRSymbol_set_is_busy(sym);
            if (s != sym) {
                solStack_push(stk, sym);
            }
        }
    } while ((n = solListNode_next(n)));
    while ((sym = solStack_pop(stk))) {
        if (solLRParser_compute_nonkernel_items(p, c, sym)) {
            solStack_free(stk);
            return 2;
        }
        solLRSymbol_set_is_idle(sym);
    }
    solStack_free(stk);
    return 0;
}

SolLRParser* solLRParser_new()
{
    SolLRParser *p = sol_calloc(1, sizeof(SolLRParser));
    p->gen = 0;
    p->collections = solList_new();
    if (p->collections == NULL) {
        goto oops;
    }
    solList_set_val_free_func(p->collections, &_solLRItemCol_free);
    p->col_rel = solRBTuple_new();
    if (p->col_rel == NULL) {
        goto oops;
    }
    p->col_rel->f_travelsal_act = &_solLRParser_compute_items_collections;
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
    if (p->collections) {
        solList_free(p->collections);
    }
    if (p->fields && solList_len(p->fields)) {
        SolListNode *n = solList_head(p->fields);
        do {
            solLRParserTableField_free((SolLRTableField*)(solListNode_val(n)));
        } while ((n = solListNode_next(n)));
    }
    if (p->col_rel) {
        solRBTuple_free(p->col_rel);
    }
    sol_free(p);
}

SolLRItemCol* solLRParser_generate_items_collection(SolLRParser *p, SolLRSymbol *s, int from)
{
    if (p == NULL) {
        return NULL;
    }
    SolLRItemCol *c;
    if (from == 0x4 && solList_len(p->collections)) {
        SolListNode *n = solList_head(p->collections);
        do {
            c = solListNode_val(n);
            if ((c->flag & 0x4) && c->sym == s) {
                c->flag |= 0x8; // computed
                return c;
            }
        } while ((n = solListNode_next(n)));
    }
    c = solLRItemCol_new();
    if (c == NULL) {
        goto oops;
    }
    c->state = ++p->gen;
    if (solList_add(p->collections, c) == NULL) {
        goto oops;
    }
    c->items = solList_new();
    if (c->items == NULL) {
        goto oops;
    }
    c->nc = solRBTree_new();
    if (c->nc == NULL) {
        goto oops;
    }
    c->nc->ex = p;
    solRBTree_set_compare_func(c->nc, &_solLRParser_compare_cols);
    if (from == 0x4) {
        c->flag |= 0x4;
    }
    c->sym = s;
    return c;
oops:
    if (c) {
        solLRItemCol_free(c);
    }
    return NULL;
}

int _solLRParser_compare_cols(void *c1, void *c2, SolRBTree *tree, int flag)
{
    if (flag & 0x2) { // insert
        if (((SolLRItemCol*)c1)->state > ((SolLRItemCol*)c2)->state) {
            return 1;
        } else if (((SolLRItemCol*)c1)->state < ((SolLRItemCol*)c2)->state) {
            return -1;
        }
        return 0;
    } else {
        return solLRParser_compare_symbol(
            (SolLRParser*)(tree->ex),
            (SolLRSymbol*)c1,
            (SolLRSymbol*)(((SolLRItemCol*)c2)->sym)
            );
    }
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
        if (c1->state > c2->state) {
            return 1;
        } else if (c1->state < c2->state) {
            return -1;
        }
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
    if (((struct _SolLRTableField*)f1)->flag > ((struct _SolLRTableField*)f1)->flag) {
        return 1;
    } else if (((struct _SolLRTableField*)f1)->flag < ((struct _SolLRTableField*)f1)->flag) {
        return -1;
    }
    if (ext & 0x2) {
        sol_free((struct _SolLRTableField*)f1);
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

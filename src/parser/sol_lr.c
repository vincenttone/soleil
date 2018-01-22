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
    SolLRItem *i = (product->items + pos);
    if (i->p == NULL) {
        i->p = product;
        i->pos = pos;
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

int solLRSymbol_compute_first(SolLRSymbol *symbol, SolLRSymbol *empty)
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
					if (solLRSymbol_record_first(symbol, s) != 0) {
						return 1;
					}
				} else if (solLRSymbol_compute_first(s, empty) != 0) {
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
					if (solLRSymbol_record_first(symbol, empty) != 0) {
						return 4;
					}
				}
			}
		} while ((n = solListNode_next(n)));
	}
	solLRSymbol_set_first_computed(symbol);
    return 0;
}

int solLRSymbol_compute_follow(SolLRSymbol *symbol, SolRBTree *symbols, SolLRSymbol *empty)
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
        n = solList_head(s->productions);
        do {
            product = (SolLRProduct*)(solListNode_val(n));
            for (i = 0; i< product->len; i++) {
                s = (SolLRSymbol*)(product->r + i);
                if (s == symbol) {
                    if (product->len == (i + 1)) { // is the last of product
						// compute product nonterminal's follow
                        if (solLRSymbol_compute_follow(product->s, symbols, empty) != 0 ) {
							return 1;
						}
						if (product->s->follows) {
							if (solLRSymbol_copy_follows(product->s->follows, symbol) != 0) {
								return 2;
							}
						}
                    } else {
                        s = (SolLRSymbol*)(product->r + i + 1);
                        if (solLRSymbol_is_terminal(s)) { // is terminal, record
                            if (solLRSymbol_record_follow(symbol, s) != 0) {
								return 3;
							}
                        } else if (solLRSymbol_is_nonterminal(s)) { // nonterminal
                            // compute s's firsts, add to follow
                            if (solLRSymbol_compute_first(s, empty) != 0) {
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
                                if (solLRSymbol_compute_follow(s, symbols, empty) != 0) {
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
 * @return 0 when success, others when failed
 */
int solLRSymbol_record_first(SolLRSymbol *symbol, SolLRSymbol *first)
{
	if (symbol == NULL || first == NULL) {
		return -1;
	}
	if (symbol->firsts == NULL) {
		symbol->firsts = solRBTree_new();
		if (symbol->firsts == NULL) {
			return -2;
		}
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
 * @return 0 when success, others when failed
 */
int solLRSymbol_record_follow(SolLRSymbol *symbol, SolLRSymbol *follow)
{
	if (symbol == NULL || follow == NULL) {
		return -1;
	}
	if (symbol->follows == NULL) {
		symbol->follows = solRBTree_new();
		if (symbol->follows == NULL) {
			return -2;
		}
	}
	if (solRBTree_insert(symbol->follows, follow) != 0) {
		return 1;
	}
	return 0;
}

int _solLRSymbol_share_firsts(SolRBTree *firsts, SolRBTreeNode *node, void *symbol)
{
	SolLRSymbol *s = (SolLRSymbol*)solRBTreeNode_val(node);
	return solLRSymbol_record_first((SolLRSymbol*)symbol, s);
}

int _solLRSymbol_share_follows(SolRBTree *follows, SolRBTreeNode *node, void *symbol)
{
	SolLRSymbol *s = (SolLRSymbol*)solRBTreeNode_val(node);
	return solLRSymbol_record_follow((SolLRSymbol*)symbol, s);
}

int solLRParser_compute_items_collections(SolLRParser *p, SolLRItemCol *c)
{
    if (c == NULL || c->items == NULL) {
        return -1;
    }
    if (c->flag & SolLRItemCol_FLAG_END) {
        return 0;
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
        if (item->flag == 1) {
            continue;
        }
        // nonkernel items
        if (item->pos > item->p->len) {
            continue;
        }
        if (item->pos == item->p->len) { // the end of product
            col = solLRParser_generate_items_collection(p);
            col->flag |= SolLRItemCol_FLAG_END;
            col->sym = item->p->s; // record product nonterminal symbol when reach end
            if (solRBTree_insert(c->nc, col) != 0) {
				return 1;
			}
        } else {
            s = *(solLRProduct_find_symbol(item->p, item->pos));
            col = solRBTree_search(c->nc, s);
            if (col == NULL) {
                col = solLRParser_generate_items_collection(p);
                col->sym = s;
                if (solRBTree_insert(c->nc, col) != 0) {
					return 2;
				}
            }
            item = solLRProduct_item(item->p, (item->pos) + 1);
            solList_add(col->items, item);
            if (solLRSymbol_is_nonterminal(s)) { // gererate nonkernel items
                solLRParser_compute_nonkernel_items(p, c, s);
            }
        }
        item->flag = 1;
    } while ((n = solListNode_next(n)));
    if (solRBTree_travelsal_inorder(c->nc, solRBTree_root(c->nc), &_solLRParser_compute_items_collections, p)) {
        return 3;
    }
    return 0;
}

int _solLRParser_compute_items_collections(SolRBTree *t, SolRBTreeNode *n, void *p)
{
    if (t == NULL || n == NULL || p == NULL) {
        return -1;
    }
    SolLRItemCol *col = solRBTreeNode_val(n);
    return solLRParser_compute_items_collections((SolLRParser*)p, col);
}

int solLRParser_compute_nonkernel_items(SolLRParser *p, SolLRItemCol *c, SolLRSymbol *s)
{
    SolStack *stk = solStack_new();
    SolListNode *n = solList_head(s->productions);
    SolLRProduct *product;
    SolLRSymbol *sym;
    SolLRItem *item;
    SolLRItemCol *col;
    do {
        product = (SolLRProduct*)(solListNode_val(n));
        sym = *(product->r);
        col = solRBTree_search(c->nc, sym);
        if (col == NULL) {
            col = solLRParser_generate_items_collection(p);
            col->sym = s;
            if (solRBTree_insert(c->nc, col) != 0) {
                solStack_free(stk);
				return 1;
			}
        }
        item = solLRProduct_item(product, 1);
        solList_add(col->items, item);
        if (solLRSymbol_is_nonterminal(sym) && solLRSymbol_is_idle(sym)) {
            solLRSymbol_set_is_busy(sym);
            solStack_push(stk, sym);
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
    SolLRParser *p = sol_alloc(sizeof(SolLRParser));
    p->gen = 0;
    p->collections = solList_new();
    if (p->collections == NULL) {
        goto oops;
    }
    solList_set_val_free_func(p->collections, &_solLRItemCol_free);
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
    sol_free(p);
}

SolLRItemCol* solLRParser_generate_items_collection(SolLRParser *p)
{
    if (p == NULL) {
        return NULL;
    }
    SolLRItemCol *c = solLRItemCol_new();
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
    solRBTree_set_compare_func(c->nc, p->compare_symbol_and_col);
    solRBTree_set_insert_compare_func(c->nc, p->compare_cols);
    return c;
oops:
    if (c) {
        solLRItemCol_free(c);
    }
    return NULL;
}

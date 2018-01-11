#include "sol_lr.h"
#include "sol_stack.h"
#include "sol_rbtree_iter.h"

SolLRSymbol* solLRSymbol_new(void *symbol)
{
    SolLRSymbol *s = sol_calloc(1, sizeof(SolLRSymbol));
    s->s = s;
    return s;
}

void solLRSymbol_free(SolLRSymbol *symbol)
{
    sol_free(symbol);
}

SolLRProduct* solLRProduct_new(size_t len, SolLRSymbol *s, ...)
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
    p->r = sol_calloc(len, sizeof(SolLRSymbol));
    if (p->r == NULL) {
        solLRProduct_free(p);
        return NULL;
    }
    SolLRSymbol *sym;
    size_t i;
    va_list al;
    va_start(al, s);
    for (i = 0; i < len; i++) {
        s = va_arg(al, SolLRSymbol*);
        sym = solLRProduct_find_symbol(p, i);
        *sym = *s;
    }
    va_end(al);
    if (s->p == NULL) {
        s->p = solList_new();
        if (s->p == NULL) {
            solLRProduct_free(p);
            return NULL;
        }
    }
    solList_add(s->p, p);
    return p;
}

void solLRProduct_free(SolLRProduct *p)
{
    if (p->r) {
        sol_free(p->r);
    }
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

SolLRItemCol* solLRItemCol_new()
{
    SolLRItemCol *c = sol_calloc(1, sizeof(SolLRItemCol));
    return c;
}

void solLRItemCol_free(SolLRItemCol *c)
{
    sol_free(c);
}

int solLRSymbol_compute_nullable(SolLRSymbol *symbol)
{
	if (symbol == NULL || symbol->p == NULL) {
		return -1;
	}
	if (solLRSymbol_is_terminal(symbol) || solLRSymbol_nullable_computed(symbol)) {
		return 0;
	}
	SolListNode *n = solList_head(symbol->p);
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
			s = (product->r + i);
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
	if (symbol == NULL || symbol->p == NULL) {
		return -1;
	}
	if (solLRSymbol_first_computed(symbol)) {
		return 0;
	}
	if (solLRSymbol_is_nonterminal(symbol)) {
		SolListNode *n = solList_head(symbol->p);
		SolLRProduct *product;
		SolLRSymbol *s;
		size_t i;
		do {
			product = solListNode_val(n);
			if (product->len == 0 || product->r == NULL) {
				return -2;
			}
			for (i = 0; i < product->len; i++) {
				s = (product->r + i);
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
    if (symbols == NULL || symbol->p == NULL) {
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
        n = solList_head(s->p);
        do {
            product = (SolLRProduct*)(solListNode_val(n));
            for (i = 0; i< product->len; i++) {
                s = (SolLRSymbol*)(product + i);
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
                        s = (SolLRSymbol*)(product + i + 1);
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

int solLRItemCol_compute_items_collections(SolLRItemCol *c, SolLRItemCol* (*gen_col)(void*), void *v)
{
    if (c == NULL || c->items == NULL) {
        return -1;
    }
    if (solList_head(c->items) == NULL || solList_len(c->items) == 0) {
        return -2;
    }
    if (c->flag & SolLRItemCol_FLAG_END) {
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
            col = (*gen_col)(v);
            col->flag |= SolLRItemCol_FLAG_END;
            col->sym = item->p->s; // record product nonterminal symbol when reach end
            if (solRBTree_insert(c->nc, col) != 0) {
				return 1;
			}
        } else {
            s = solLRProduct_find_symbol(item->p, item->pos);
            col = solRBTree_search(c->nc, s);
            if (col == NULL) {
                col = (*gen_col)(v);
                col->sym = s;
                if (solRBTree_insert(c->nc, col) != 0) {
					return 1;
				}
            }
            item = solLRItem_new(item->p, (item->pos) + 1);
            solList_add(col->items, item);
            if (solLRSymbol_is_nonterminal(s)) { // gererate nonkernel items
                solLRItemCol_compute_nonkernel_items(c, s, gen_col, v);
            }
        }
    } while ((n = solListNode_next(n)));
    SolRBTreeIter* rbti = solRBTreeIter_new(c->nc, solRBTree_root(c->nc), SolRBTreeIterTT_inorder);
    do { // compute next kernel items collections
        col = (SolLRItemCol*)(solRBTreeIter_current_val(rbti));
        // compute next step items collections
        if (solLRItemCol_compute_items_collections(col, gen_col, v) != 0) {
            return 1;
        }
    } while (solRBTreeIter_next(rbti) != NULL);
    return 0;
}

int solLRItemCol_compute_nonkernel_items(SolLRItemCol *c, SolLRSymbol *s, SolLRItemCol* (*gen_col)(void*), void *v)
{
    SolStack *stk = solStack_new();
    SolListNode *n = solList_head(s->p);
    SolLRProduct *product;
    SolLRSymbol *sym;
    SolLRItem *item;
    SolLRItemCol *col;
    do {
        product = (SolLRProduct*)(solListNode_val(n));
        sym = solLRProduct_right(product);
        col = solRBTree_search(c->nc, sym);
        if (col == NULL) {
            col = (*gen_col)(v);
            col->sym = s;
            if (solRBTree_insert(c->nc, col) != 0) {
                solStack_free(stk);
				return 1;
			}
        }
        item = solLRItem_new(product, 1);
        solList_add(col->items, item);
        if (solLRSymbol_is_nonterminal(sym) && solLRSymbol_is_idle(sym)) {
            solLRSymbol_set_is_busy(sym);
            solStack_push(stk, sym);
        }
    } while ((n = solListNode_next(n)));
    while ((sym = solStack_pop(stk))) {
        if (solLRItemCol_compute_nonkernel_items(c, sym, gen_col, v)) {
            return 2;
        }
        solLRSymbol_set_is_idle(sym);
    }
    solStack_free(stk);
    return 0;
}

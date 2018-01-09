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
    p->table = solRBTuple_new();
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
            if (solRBTree_insert(c->nc, col) != 0) {
				return 1;
			}
        } else {
            s = solLRProduct_find_symbol(item->p, item->pos);
            col = solRBTree_search(c->nc, s);
            if (col == NULL) {
                col = solSLRParser_generate_items_collection();
                col->sym = s;
                if (solRBTree_insert(c->nc, col) != 0) {
					return 1;
				}
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
            if (solRBTree_insert(c->nc, col) != 0) {
				return 1;
			}
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
    struct _SolSLRTableField *s = sol_calloc(1, sizeof(struct _SolSLRTableField));
    s->t = state;
    s->flag |= SolLRTableFieldFlag_TYPE_STATE;
    struct _SolSLRTableField *sym = sol_calloc(1, sizeof(struct _SolSLRTableField));
    sym->t = p->s;
    sym->flag |= SolLRTableFieldFlag_TYPE_SYMBOL;
    if (solRBTuple_put(p->table, s, sym) != 0) {
        return 1;
    }
    return 0;
}

int solSLRParser_record_reduce(SolSLRParser *p, size_t state, SolLRSymbol *symbol)
{
    if (solSLRParser_compute_follow(p, symbol) != 0) {
        return 1;
    }
    struct _SolSLRTableField *s = sol_calloc(1, sizeof(struct _SolSLRTableField));
    s->t = state1;
    s->flag |= SolLRTableFieldFlag_TYPE_STATE;
    struct _SolSLRTableField *sym1 = sol_calloc(1, sizeof(struct _SolSLRTableField));
    sym1->t = symbol;
    sym1->flag |= SolLRTableFieldFlag_TYPE_SYMBOL;
    sym2->flag |= SolLRTableFieldFlag_ACTION_REDUCE;
    struct _SolSLRTableField *sym2;
    SolRBTreeIter *i = solRBTreeIter_new(symbol->follows, solRBTree_root(symbol->follows), SolRBTreeIterTT_inorder);
    do {
        symbol = (SolLRSymbol*)(solRBTreeIter_current_val(i));
        sym2 = sol_calloc(1, sizeof(struct _SolSLRTableField));
        sym2->t = symbol;
        sym2->flag |= SolLRTableFieldFlag_TYPE_SYMBOL;
        if (solRBTuple_put(p->table, s, sym2, sym1) != 0) {
            return 1;
        }
    } while (solRBTreeIter_next(i));
    return 0;
}

int solSLRParser_record_shift(SolSLRParser *p, size_t state1, SolLRSymbol *symbol, size_t state2)
{
    struct _SolSLRTableField *s1 = sol_calloc(1, sizeof(struct _SolSLRTableField));
    s->t = state1;
    s->flag |= SolLRTableFieldFlag_TYPE_STATE;
    struct _SolSLRTableField *sym = sol_calloc(1, sizeof(struct _SolSLRTableField));
    sym->t = symbol;
    sym->flag |= SolLRTableFieldFlag_TYPE_SYMBOL;
    struct _SolSLRTableField *s2 = sol_calloc(1, sizeof(struct _SolSLRTableField));
    s->t = state2;
    s->flag |= SolLRTableFieldFlag_TYPE_STATE;
    s->flag |= SolLRTableFieldFlag_ACTION_SHIFT;
    if (solRBTuple_put(p->table, s1, sym, s2) != 0) {
        return 1;
    }
    return 0;
}

int solSLRParser_record_goto(SolSLRParser *p, size_t state1, SolLRSymbol *symbol, size_t state2)
{
    struct _SolSLRTableField *s1 = sol_calloc(1, sizeof(struct _SolSLRTableField));
    s->t = state1;
    s->flag |= SolLRTableFieldFlag_TYPE_STATE;
    struct _SolSLRTableField *sym = sol_calloc(1, sizeof(struct _SolSLRTableField));
    sym->t = symbol;
    sym->flag |= SolLRTableFieldFlag_TYPE_SYMBOL;
    struct _SolSLRTableField *s2 = sol_calloc(1, sizeof(struct _SolSLRTableField));
    s->t = state2;
    s->flag |= SolLRTableFieldFlag_TYPE_STATE;
    s->flag |= SolLRTableFieldFlag_ACTION_GOTO;
    if (solRBTuple_put(p->table, s1, sym, s2) != 0) {
        return 1;
    }
    return 0;
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

int solSLRParser_compute_first(SolSLRParser *p, SolSymbol *symbol)
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
				} else if (solSLRParser_compute_first(p, s) != 0) {
					if (s->firsts) {
						if (solLRSymbol_copy_firsts(s-firsts, symbol) != 0) {
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
					if (solLRSymbol_record_first(p, symbol, p->e) != 0) {
						return 4;
					}
				}
			}
		} while ((n = solListNode_next(n)));
	}
	solLRSymbol_set_first_computed(s);
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
                    if (product->len == (i + 1)) { // is the last of product
						// compute product nonterminal's follow
                        if (solSLRParser_compute_follow(p, product->s) != 0 ) {
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
                            if (solLRSymbol_record_follow(p, s) != 0) {
								return 3;
							}
                        } else if (solLRSymbol_is_nonterminal(s)) { // nonterminal
                            // compute s's firsts, add to follow
                            if (solSLRParser_compute_first(p, s) != 0) {
								return 1;
							}
							// add s's first to symbol's follow
							if (s->firsts) {
								if (solLRSymbol_copy_follows(s->firsts, symbol) != 0) {
									return 2;
								}
							}
                            if (solSLRParser_compute_nullable(p, s) != 0) {
								return 3;
							}
                            if (solLRSymbol_is_nullable(s) == 0) {
                                if (solSLRParser_compute_follow(p, s) != ) {
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
    } while ((solRBTreeIter_next(iter)));
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
	if (solRBTree_insert(symbol->follows, follows) != 0) {
		return 1;
	}
	return 0;
}

int solLRSymbol_share_firsts(SolRBTree *firsts, SolRBTreeNode *node, void *symbol)
{
	SolLRSymbol *s = (SolLRSymbol*)solRBTreeNode_val(node);
	return solLRSymbol_record_first((SolLRSymbol*)symbol, s);
}

int solLRSymbol_share_follows(SolRBTree *follows, SolRBTreeNode *node, void *symbol)
{
	SolLRSymbol *s = (SolLRSymbol*)solRBTreeNode_val(node);
	return solLRSymbol_record_follow((SolLRSymbol*)symbol, s);
}

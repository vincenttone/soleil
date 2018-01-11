#include <assert.h>
#include "sol_rb_tuple.h"

SolRBTuple* solRBTuple_new()
{
    SolRBTuple *t = sol_calloc(1, sizeof(SolRBTuple));
	if (t == NULL) {
		return NULL;
	}
    t->n = solRBTree_new();
    t->tmp = sol_calloc(1, sizeof(SolRBTuple));
    solRBTree_set_val_free_func(t->n, &_solRBTupleRecord_free);
	return t;
}

void solRBTuple_free(SolRBTuple *t)
{
    if (t->tmp) {
        sol_free(t->tmp);
    }
    if (t->n) {
        solRBTree_free(t->n);
    }
    sol_free(t);
}

SolRBTupleRecord* solRBTupleRecord_new(SolRBTuple *t, void *v)
{
    SolRBTupleRecord *r = sol_calloc(1, sizeof(SolRBTuple));
	if (r == NULL) {
		return NULL;
	}
    if (t->f_free_val) {
        solRBTupleRecord_set_free_val_func(r, t->f_free_val);
    }
    r->v = v;
	return r;
}

void _solRBTupleRecord_free(void *r)
{
	return solRBTupleRecord_free((SolRBTupleRecord*)r);
}

void solRBTupleRecord_free(SolRBTupleRecord *r)
{
	if (r->v != NULL && r->f_free_val) {
		(*r->f_free_val)(r->v);
	}
	if (r->n) {
		solRBTree_free(r->n);
	}
    sol_free(r);
}

int solRBTuple_put(SolRBTuple *t, size_t l, ...)
{
	va_list al;
	va_start(al, l);
    SolRBTree *tree = t->n;
    SolRBTupleRecord *pre;
	SolRBTupleRecord *cur;
    void *v;
	 while (l--) {
		v = va_arg(al, void*);
        if (tree) {
            t->tmp->v = v;
            cur = (SolRBTupleRecord*)(solRBTree_search(tree, t->tmp));
        } else {
            cur = NULL;
            pre->n = solRBTree_new();
            if (pre->n == NULL) {
                return 1;   
            }
            tree = pre->n;
            solRBTree_set_compare_func(tree, solRBTree_node_val_compare_func(t->n));
            solRBTree_set_insert_conflict_fix_func(tree, solRBTree_insert_conflict_fix_func(t->n));
            solRBTree_set_val_free_func(tree, solRBTree_node_val_free_func(t->n));
        }
		if (cur == NULL) {
            cur = solRBTupleRecord_new(t, v);
            if (cur == NULL) {
                return 1;
            }
            solRBTree_insert(tree, cur);
		}
        tree = cur->n;
		pre = cur;
	}
	va_end(al);
	return 0;
}

SolRBTupleRecord* solRBTuple_get(SolRBTuple *t, size_t l, ...)
{
	if (t == NULL) {
		return NULL;
	}
    SolRBTree *tree = t->n;
    SolRBTupleRecord *r;
    void *v;
	va_list al;
	va_start(al, l);
    while (l--) {
        if (tree == NULL) {
            return NULL;
        }
		v = va_arg(al, void*);
        t->tmp->v = v;
		r = (SolRBTupleRecord*)(solRBTree_search(tree, t->tmp));
		if (r == NULL) {
			return NULL;
		}
        tree = r->n;
	}
	va_end(al);
	return r;
}

int solRBTuple_remove(SolRBTuple *t, size_t l, ...)
{
	if (t == NULL) {
		return -1;
	}
    SolRBTree *tree = t->n;
    SolRBTree *target_tree;
    SolRBTupleRecord *r;
    SolRBTreeNode *n;
    void *v;
	va_list al;
	va_start(al, l);
    while (l--) {
        if (tree == NULL) {
            return 1;
        }
        target_tree = tree;
		v = va_arg(al, void*);
        t->tmp->v = v;
        n = solRBTree_search_node(tree, t->tmp);
		r = (SolRBTupleRecord*)(solRBTreeNode_val(n));
		if (r == NULL) {
            va_end(al);
			return 2;
		}
        tree = r->n;
	}
	va_end(al);
    if (solRBTree_delete_node(target_tree, n) != 0) {
        return 3;
    }
	return 0;
}

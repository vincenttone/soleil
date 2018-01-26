#include <assert.h>
#include "sol_rb_tuple.h"

SolRBTuple* solRBTuple_new()
{
    SolRBTuple *t = sol_calloc(1, sizeof(SolRBTuple));
	if (t == NULL) {
		return NULL;
	}
    t->n = solRBTree_new();
    t->n->ex = t;
    solRBTree_set_compare_func(t->n, &_solRBTuple_compare_node_val);
    solRBTree_set_val_free_func(t->n, &_solRBTupleRecord_free);
	return t;
}

void solRBTuple_free(SolRBTuple *t)
{
    if (t->n) {
        solRBTree_free(t->n);
    }
    sol_free(t);
}

int _solRBTuple_compare_node_val(void *r1, void *r2, SolRBTree *tree, int flag)
{
    SolRBTuple *t = (SolRBTuple*)(tree->ex);
    if (flag & 0x2) { // insert
        return (*t->f_cmp_val)(
            ((SolRBTupleRecord*)r1)->v,
            ((SolRBTupleRecord*)r2)->v,
            t,
            flag
            );
    }
    return (*t->f_cmp_val)(
        r1,
        ((SolRBTupleRecord*)r2)->v,
        t,
        flag
        );
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
            cur = (SolRBTupleRecord*)(solRBTree_search(tree, v));
        } else {
            cur = NULL;
            pre->n = solRBTree_new();
            if (pre->n == NULL) {
                return 1;   
            }
            tree = pre->n;
            tree->ex = t;
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
		r = (SolRBTupleRecord*)(solRBTree_search(tree, v));
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
        n = solRBTree_search_node(tree, v);
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

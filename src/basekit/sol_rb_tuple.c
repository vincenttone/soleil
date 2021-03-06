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
    SolRBTupleRecord *pre = NULL;
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
            solRBTree_set_val_free_func(tree, solRBTree_node_val_free_func(t->n));
        }
        if (cur == NULL) {
            cur = solRBTupleRecord_new(t, v);
            if (cur == NULL) {
                return 1;
            }
            if (pre == NULL) {
                cur->level = 0;
            } else {
                cur->level = pre->level + 1;
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

void* solRBTuple_get_first(SolRBTuple *t, size_t l, ...)
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
    if (r && r->n && solRBTree_count(r->n)) {
        r = solRBTree_min(r->n);
        if (r) {
            return r->v;
        }
    }
	return NULL;
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

int solRBTuple_travelsal(SolRBTuple *t, int (*f_travelsal_act)(void*, struct _SolRBTuple*, size_t, void*), void *d)
{
    struct _SolRBTupleTravelsalData* data = sol_alloc(sizeof(struct _SolRBTupleTravelsalData));
    data->f_travelsal_act = f_travelsal_act;
    data->ext = d;
    if (solRBTree_travelsal_inorder(t->n, solRBTree_root(t->n), &_solRBTupleRecord_travelsal, data)) {
        sol_free(data);
        return 1;
    }
    sol_free(data);
    return 0;
}

int solRBTuple_record_travelsal(SolRBTuple *t, SolRBTupleRecord *r, void *d)
{
    if (solRBTree_travelsal_inorder(r->n, solRBTree_root(r->n), &_solRBTupleRecord_travelsal, d)) {
        return 1;
    }
    return 0;
}

int _solRBTupleRecord_travelsal(SolRBTree *tree, SolRBTreeNode *node, void *d)
{
    SolRBTupleRecord *r = solRBTreeNode_val(node);
    if ((*(((struct _SolRBTupleTravelsalData*)d)->f_travelsal_act))(
            r->v,
            ((SolRBTuple*)tree->ex),
            r->level,
            (((struct _SolRBTupleTravelsalData*)d))->ext
            ) != 0
        ) {
        return 1;
    }
    if (r->n) {
        if (solRBTree_travelsal_inorder(r->n, solRBTree_root(r->n), &_solRBTupleRecord_travelsal, d) != 0) {
            return 2;
        }
    }
    return 0;
}

int solRBTuple_list(SolRBTuple *t, int (*f_list_act)(SolList*, struct _SolRBTuple*, void*), void *d)
{
    struct _SolRBTupleListData* data = sol_alloc(sizeof(struct _SolRBTupleTravelsalData));
    data->f_list_act = f_list_act;
    data->ext = d;
    data->list = solList_new();
    if (solRBTree_travelsal_inorder(t->n, solRBTree_root(t->n), &_solRBTupleRecord_travelsal_list, data)) {
        sol_free(data);
        return 1;
    }
    if ((*f_list_act)(data->list, t, d) != 0) {
        return 1;
    }
    solList_free(data->list);
    sol_free(data);
    return 0;
}

int _solRBTupleRecord_travelsal_list(SolRBTree *tree, SolRBTreeNode *node, void *d)
{
    SolRBTupleRecord *r = solRBTreeNode_val(node);
    SolList *list = ((struct _SolRBTupleListData*)d)->list;
    if (r->level < solList_len(list)) {
        if ((*(((struct _SolRBTupleListData*)d)->f_list_act))(
                list,
                ((SolRBTuple*)tree->ex),
                (((struct _SolRBTupleListData*)d))->ext
                ) != 0
            ) {
            return 1;
        }
        do {
            if (solList_del_node(list, solList_tail(list))) {
                return 1;
            }
        } while (r->level < solList_len(list));
    }
    if (solList_add(list, r->v) == NULL) {
        return 2;
    }
    if (r->n) {
        if (solRBTree_travelsal_inorder(r->n, solRBTree_root(r->n), &_solRBTupleRecord_travelsal_list, d) != 0) {
            return 2;
        }
    }
    return 0;
}

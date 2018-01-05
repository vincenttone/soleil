#include <assert.h>
#include "sol_rb_tuple.h"

SolRBTuple* solRBTuple_new()
{
	SolRBTuple *t = _solRBTuple_new(NULL, NULL);
	if (t == NULL) {
		return NULL;
	}
	return t;
}

SolRBTuple* _solRBTuple_new(SolRBTuple *parent, void *v)
{
    SolRBTuple *t = sol_calloc(1, sizeof(SolRBTuple));
    if (t == NULL) {
        return NULL;
    }
	t->v = v;
	if (parent) {
		t->p = parent;
		solRBTuple_set_free_val_func(parent, parent->f_free_val);
		if (parent->n == NULL)  {
			parent->n = solRBTree_new();
			if (parent->p && parent->p->n) {
				SolRBTree *grand = parent->p->n;
				solRBTuple_set_compare_val_func(parent, solRBTree_node_val_compare_func(grand));
				solRBTuple_set_insert_conflict_fix_func(parent, solRBTree_insert_conflict_fix_func(grand));
				solRBTree_set_val_free_func(parent->n, &_solRBTuple_free);
			}
		}
		solRBTree_insert(parent->n, t);
	}
    return t;
}

void _solRBTuple_free(void *t)
{
	return solRBTuple_free((SolRBTuple*)t);
}

void solRBTuple_free(SolRBTuple *t)
{
	if (t->v && t->f_free_val) {
		(*t->f_free_val)(t->v);
	}
	if (t->n) {
		solRBTree_free(t->n);
	}
    sol_free(t);
}

SolRBTuple* solRBTuple_put(SolRBTuple *t, size_t l, void *v, ...)
{
	SolRBTree *current = t->n;
	va_list al;
	va_start(al, v);
	SolRBTuple *nt;
	nt = (SolRBTuple*)(solRBTree_search(current, v));
	if (nt == NULL) {
		nt = _solRBTuple_new(t, v);
	}
	t = nt;
	while (--l) {
		v = va_arg(al, void*);
		nt = (SolRBTuple*)(solRBTree_search(current, v));
		if (nt == NULL) {
			nt = _solRBTuple_new(t, v);
		}
		t = nt;
	}
	va_end(al);
	return nt;
}

void* solTuple_get(SolRBTuple *t, size_t l, void *v, ...)
{
	if (t == NULL) {
		return NULL;
	}
	t = (SolRBTuple*)(solRBTree_search(t->n, v));
	if (t == NULL) {
		return NULL;
	}
	va_list al;
	va_start(al, v);
	while (--l) {
		v = va_arg(al, void*);
		t = (SolRBTuple*)(solRBTree_search(t->n, v));
		if (t == NULL) {
			return NULL;
		}
	}
	va_end(al);
	return t->v;
}

int solRBTuple_remove(SolRBTuple *t, size_t l, void *v, ...)
{
	if (t == NULL) {
		return -1;
	}
	t = (SolRBTuple*)(solRBTree_search(t->n, v));
	if (t == NULL) {
		return 0;
	}
	va_list al;
	va_start(al, v);
	while (--l) {
		v = va_arg(al, void*);
		t = (SolRBTuple*)(solRBTree_search(t->n, v));
		if (t == 0) {
			return 0;
		}
	}
	va_end(al);
	solRBTuple_free(t);
	return 0;
}

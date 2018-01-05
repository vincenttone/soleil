#ifndef _SOL_RB_TUPLE_H_
#define _SOL_RB_TUPLE_H__ 1

#include <stdarg.h>
#include "sol_common.h"
#include "sol_rbtree.h"

typedef struct _SolRBTuple {
	void *v;
	SolRBTree *n;
	struct _SolRBTuple *p; // parent
	sol_f_free_ptr f_free_val; // free node val func
} SolRBTuple;

SolRBTuple* solRBTuple_new();
SolRBTuple* _solRBTuple_new(SolRBTuple*, void*);
void solRBTuple_free(SolRBTuple*);
void _solRBTuple_free(void *t);

SolRBTuple* solRBTuple_put(SolRBTuple*, size_t, void*, ...);
void* solRBTuple_get(SolRBTuple*, size_t, void*, ...);
int solRBTuple_remove(SolRBTuple*, size_t, void*, ...);

#define solRBTuple_set_compare_val_func(t, f) solRBTree_set_compare_func(t->n, f)
#define solRBTuple_set_insert_conflict_fix_func(t, f) solRBTree_set_insert_conflict_fix_func(t->n, f)
#define solRBTuple_set_free_val_func(t, f) t->f_free_val = f

#endif

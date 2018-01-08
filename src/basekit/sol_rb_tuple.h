#ifndef _SOL_RB_TUPLE_H_
#define _SOL_RB_TUPLE_H__ 1

#include <stdarg.h>
#include "sol_common.h"
#include "sol_rbtree.h"

typedef struct _SolRBTupleRecord {
	void *v;
	SolRBTree *n;
    sol_f_free_ptr f_free_val; // free node val func
} SolRBTupleRecord;

typedef struct _SolRBTuple {
	SolRBTree *n;
    struct _SolRBTupleRecord *tmp;
    sol_f_free_ptr f_free_val; // free node val func
} SolRBTuple;

SolRBTuple* solRBTuple_new();
void solRBTuple_free(SolRBTuple*);
SolRBTupleRecord* solRBTupleRecord_new(SolRBTuple *t, void*);
void solRBTupleRecord_free(SolRBTupleRecord*);
void _solRBTupleRecord_free(void*);

int solRBTuple_put(SolRBTuple*, size_t, ...);
SolRBTupleRecord* solRBTuple_get(SolRBTuple*, size_t, ...);
int solRBTuple_remove(SolRBTuple*, size_t, ...);

#define solRBTuple_set_compare_val_func(t, f) solRBTree_set_compare_func((t)->n, f)
#define solRBTupleRecord_set_free_val_func(r, f) (r)->f_free_val = f

#endif

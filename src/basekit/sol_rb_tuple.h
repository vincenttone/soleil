#ifndef _SOL_RB_TUPLE_H_
#define _SOL_RB_TUPLE_H_ 1

#include <stdarg.h>
#include "sol_common.h"
#include "sol_rbtree.h"
#include "sol_list.h"

typedef struct _SolRBTupleRecord {
    size_t level;
	void *v;
	SolRBTree *n;
    sol_f_free_ptr f_free_val; // free node val func
} SolRBTupleRecord;

typedef struct _SolRBTuple {
	SolRBTree *n;
    void *ex; // for extend
    int (*f_cmp_val)(void*, void*, struct _SolRBTuple*, int);
    sol_f_free_ptr f_free_val; // free node val func
} SolRBTuple;

struct _SolRBTupleTravelsalData {
    void* ext;
    SolList *list;
    int (*f_travelsal_act)(void*, struct _SolRBTuple*, size_t, void*);
};

struct _SolRBTupleListData {
    void* ext;
    SolList *list;
    int (*f_list_act)(SolList*, struct _SolRBTuple*, void*);
};

SolRBTuple* solRBTuple_new();
void solRBTuple_free(SolRBTuple*);
SolRBTupleRecord* solRBTupleRecord_new(SolRBTuple *t, void*);
void solRBTupleRecord_free(SolRBTupleRecord*);
void _solRBTupleRecord_free(void*);

int _solRBTuple_compare_node_val(void*, void*, SolRBTree*, int);

int solRBTuple_put(SolRBTuple*, size_t, ...);
SolRBTupleRecord* solRBTuple_get(SolRBTuple*, size_t, ...);
int solRBTuple_remove(SolRBTuple*, size_t, ...);
void* solRBTuple_get_first(SolRBTuple*, size_t, ...);

int solRBTuple_travelsal(SolRBTuple*, int (*f_travelsal_act)(void*, struct _SolRBTuple*, size_t, void*), void*);
int _solRBTupleRecord_travelsal(SolRBTree*, SolRBTreeNode*, void*);

int solRBTuple_record_travelsal(SolRBTuple*, SolRBTupleRecord*, void*);

int solRBTuple_list(SolRBTuple*, int (*f_list_act)(SolList*, struct _SolRBTuple*, void*), void*);
int _solRBTupleRecord_travelsal_list(SolRBTree*, SolRBTreeNode*, void*);

#define solRBTuple_set_compare_val_func(t, f) ((t)->f_cmp_val = f)
#define solRBTuple_set_free_val_func(t, f) ((t)->f_free_val = f)

#define solRBTupleRecord_set_free_val_func(r, f) (r)->f_free_val = f

#endif

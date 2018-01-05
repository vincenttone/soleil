#ifndef _SOL_TRIPLE_H_
#define _SOL_TRIPLE_H_ 1

#include "sol_common.h"
#include "sol_rbtree.h"

struct _SolTripleRecordF1 {
	void *v1;
	SolRBTree *t;
};
struct _SolTripleRecordF2 {
	void *v2;
	void *v3;
};

typedef struct _SolTriple {
    SolRBTree *t;
	SolRBTreeF1 *tmp_f1;
	sol_f_cmp_ptr f_compare_v2;
	sol_f_free_ptr f_free_v2;
	sol_f_free_ptr f_free_v3;
} SolTriple;

SolTriple* solTriple_new();
void solTriple_free(SolTriple*);

int solTriple_put(SolTriple*, void*, void*, void*);
SolList* solTriple_get_by_v1(SolTriple*, void*);
void* solTriple_get(SolTriple*, void*, void*);

int solTriple_remove_by_v1(SolTriple*, void*);
int solTriple_remove(SolTriple*, void*, void*);

#define solTripleRecord_v1(r) r->v1
#define solTripleRecord_v2(r) r->v2
#define solTripleRecord_next(r) r->n

#endif

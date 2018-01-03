#ifndef _SOL_TRIPLE_H_
#define _SOL_TRIPLE_H_ 1

#include "sol_common.h"
#include "sol_hash.h"
#include "sol_list.h"

typedef struct _SolTripleK {
	void *v1;
	void *v2;
} SolTripleK;

typedef struct _SolTriple {
    SolHash *h1;
	SolHash *h2;
	SolTripleK *tmp_k;
    sol_f_hash_ptr f_v1_hash1;
    sol_f_hash_ptr f_v1_hash2;
    sol_f_hash_ptr f_v2_hash1;
    sol_f_hash_ptr f_v2_hash2;
    sol_f_match_ptr f_v1_match;
    sol_f_match_ptr f_v2_match;
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

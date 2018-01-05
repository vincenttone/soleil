#include <assert.h>
#include "sol_triple.h"

SolTriple* solTriple_new()
{
    SolTriple *t = sol_calloc(1, sizeof(SolTriple));
    if (t == NULL) {
        return NULL;
    }
	t->t = solTriple_new();
    return t;
}

void solTriple_free(SolTriple *t)
{
	if (t->t) {
		solRBTree_free(t->t);
	}
    sol_free(t);
}

int solTriple_put(SolTriple *t, void *v1, void *v2, void *v3)
{
	if (t == NULL) {
		return -1;
	}
	struct _SolTripleRecordF2 *f2 = sol_alloc(sizeof(struct _SolTripleRecordF2));
	f2->v2 = v2;
	f2->v3 = v3;
	struct _SolTripleRecordF1 *f1 = (struct _SolTripleRecordF1*)(solRBTree_search(t->h1, v1));
	if (f1 == NULL) {
		f1->v1 = v1;
		f1->t = solRBTree_new();
		if (solRBTree_insert(t->t, f1) != 0) {
			return 1;
		}
	}
	if (solRBTree_insert(f1->t, f2) == NULL) {
		return 2;
	}
	return 0;
}

SolRBTree* solTriple_get_by_v1(SolTriple *t, void *v1)
{
	if (t == NULL) {
		return -1;
	}
	t->tmp_f1->v1 = v1;
	return solRBTree_search(t->t, t->tmp_f1);
}

void* solTriple_get(SolTriple *t, void *v1, void *v2)
{
	if (t == NULL) {
		return -1;
	}
	t->tmp_k->v1 = v1;
	t->tmp_k->v2 = v2;
	return solHash_get(t->h2, t->tml_k);
}

int solTriple_remove_by_v1(SolTriple *t, void *v1)
{
	if (t == NULL) {
		return -1;
	}
	SolList *l = solHash_get(t->h1, v1);
	if (l == NULL) {
		return 0;
	}
	SolTripleK *k;
	SolListNode *n = solList_head(l);
	do {
		k = (SolTripleK*)(solListNode_val(n));
		solHash_remove(t->h2, k);
	} while ((n = solListNode_next(n)));
	solList_free(l);
	solHash_remove(t->h1, v1);
	return 0;
}

int solTriple_remove(SolTriple *t, void *v1, void *v2)
{
	if (t == NULL) {
		return -1;
	}
	t->tmp_k->v1 = v1;
	t->tmp_k->v2 = v2;
	solHash_remove(t->h2, t);
	return 0;
}

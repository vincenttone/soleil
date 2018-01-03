#include <assert.h>
#include "sol_triple.h"

SolTriple* solTriple_new()
{
    SolTriple *t = sol_calloc(1, sizeof(SolTriple));
    if (t == NULL) {
        return NULL;
    }
	t->h1 = solHash_new();
	t->h2 = solHash_new();
	t->tmp_k = sol_alloc(sizeof(SolTripleK));
    return t;
}

void solTriple_free(SolTriple *t)
{
	if (t->h1) {
		sol_free(t->h1);
	}
	if (t->h2) {
		sol_free(t->h2);
	}
    sol_free(t);
}

int solTriple_put(SolTriple *t, void *v1, void *v2, void *v3)
{
	if (t == NULL) {
		return -1;
	}
	SolTripleK *k = sol_alloc(sizeof(SolTripleK));
	k->v1 = v1;
	k->v2 = v2;
	SolList *l = solHash_get(t->h1, v1);
	if (l == NULL) {
		l = solList_new();
		if (solHash_put(t->h1, v1, l) != 0) {
			return 1;
		}
	}
	if (solList_add(l, k) == NULL) {
		return 2;
	}
	if (solHash_put(t->h2, k, v3) != 0) {
		return 3;
	}
	return 0;
}

SolList* solTriple_get_by_v1(SolTriple *t, void *v1)
{
	if (t == NULL) {
		return -1;
	}
	return solHash_get(t->h1, v1);
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

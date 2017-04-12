#include "sol_set.h"

SolSet* solSet_new()
{
	SolHash *hash = solHash_new();
	SolHashIter *iter = solHashIter_new(hash);
	SolSet *s = sol_alloc(sizeof(SolSet));
	s->hash = hash;
	s->iter = iter;
	return s;
}

void solSet_free(SolSet *s)
{
	solHashIter_free(s->iter);
	solHash_free(s->hash);
	sol_free(s);
}

inline void* solSet_current(SolSet *s)
{
	SolHashRecord *r = solHashIter_current_record(s->iter);
	if (r == NULL) {
		return NULL;
	}
	return r->k;
}

void* solSet_get(SolSet *s)
{
	void *v;
	do {
		v = solSet_current(s);
		solSetIter_next(s);
		if (v) {
			return v;
		}
	} while (solSetIter_current_count(s) < solSet_size(s));
	return NULL;
}

int solSet_is_subset(SolSet *s, SolSet *s1)
{
	void *v;
	while ((v = solSet_get(s1))) {
		if (!solSet_in_set(s, v)) {
			return 1;
		}
	}
	return 0;
}

int solSet_equal(SolSet *s1, SolSet *s2)
{
	if (solSet_count(s1) != solSet_count(s2)) {
		return 1;
	}
	return solSet_is_subset(s1, s2);
}

int solSet_merge(SolSet *s, SolSet *s1)
{
	void *v;
	while ((v = solSet_get(s1))) {
		if (solSet_add(s, v) == 1) {
			return 1;
		}
	}
	return 0;
}

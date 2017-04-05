#include "sol_set.h"

SolSet* solSet_new()
{
	SolHash *hash = sol_hash_new();
	SolHashIter *iter = sol_hash_iter_new(hash);
	SolSet *s = sol_alloc(sizeof(SolSet));
	s->hash = hash;
	s->iter = iter;
	return s;
}

void solSet_free(SolSet *s)
{
	sol_hash_iter_free(s->iter);
	sol_hash_free(s->hash);
	sol_free(s);
}

inline void* solSet_current(SolSet *s)
{
	SolHashRecord *r = sol_hash_iter_current_record(s->iter);
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
		solSet_next(s);
		if (v) {
			return v;
		}
	} while (v == NULL && s->iter->num < solSet_size(s));
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

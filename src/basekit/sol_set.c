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

void* solSet_get_value(SolSet *s)
{
	void *v;
	do {
		v = solSet_current(s);
		if (v) {
			return v;
		}
		solSet_next(s);
	} while (v == NULL && s->iter->num < solSet_size(s));
	return NULL;
}

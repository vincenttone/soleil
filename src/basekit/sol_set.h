#ifndef _SOL_SET_H_
#define _SOL_SET_H_ 1

#include <stddef.h>
#include "sol_common.h"
#include "sol_hash.h"

typedef struct SolSet {
	SolHash *hash;
	SolHashIter *iter;
} SolSet;

SolSet* solSet_new();
void solSet_free(SolSet*);

#define solSet_size(s) solHash_size(s->hash)
#define solSet_set_hash_func1(s, f) solHash_set_hash_func1(s->hash, f)
#define solSet_set_hash_func2(s, f) solHash_set_hash_func2(s->hash, f)
#define solSet_set_equal_func(s, f) solHash_set_equal_func(s->hash, f)

#define solSet_count(s) solHash_count(s->hash)
#define solSet_add(s, v) solHash_put_key_and_val(s->hash, v, SolNil)
#define solSet_remove(s, v) solHash_remove_by_key(s->hash, v)
#define solSet_in_set(s, v) solHash_has_key(s->hash, v)
#define solSet_is_empty(s) (solSet_count(s) == 0)
#define solSet_is_not_empty(s) (solSet_count(s) > 0)

void* solSet_get(SolSet*);
int solSet_is_subset(SolSet*, SolSet*);
int solSet_equal(SolSet*, SolSet*);
int solSet_merge(SolSet*, SolSet*);

#define solSet_rewind(s) solHashIter_rewind(s->iter)
#define solSet_next(s) solHashIter_next(s->iter)
#define solSetIter_current_count(s) s->iter->c
inline void* solSet_current(SolSet*);

#endif

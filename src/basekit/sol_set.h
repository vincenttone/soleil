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

#define solSet_size(s) sol_hash_size(s->hash)
#define solSet_set_hash_func1(s, f) sol_hash_set_hash_func1(s->hash, f)
#define solSet_set_hash_func2(s, f) sol_hash_set_hash_func2(s->hash, f)
#define solSet_set_equal_func(s, f) sol_hash_set_equal_func(s->hash, f)

#define solSet_count(s) sol_hash_count(s->hash)
#define solSet_add(s, v) sol_hash_put_key_and_val(s->hash, v, SolNil)
#define solSet_remove(s, v) sol_hash_remove_by_key(s->hash, v)
#define solSet_in_set(s, v) sol_hash_has_key(s->hash, v)

void* solSet_get_value(SolSet*);

#define solSet_rewind(s) sol_hash_iter_rewind(s->iter)
#define solSet_next(s) sol_hash_iter_next(s->iter)
inline void* solSet_current(SolSet*);

#endif

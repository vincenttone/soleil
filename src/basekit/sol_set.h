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
#define solSet_hash_func1(s) solHash_hash_func1(s->hash)
#define solSet_hash_func2(s) solHash_hash_func2(s->hash)
#define solSet_equal_func(s) solHash_equal_func(s->hash)
#define solSet_free_func(s) solHash_free_k_func(s->hash)
#define solSet_set_equal_func(s, f) solHash_set_equal_func(s->hash, f)
#define solSet_set_free_func(s, f) solHash_set_free_k_func(s->hash, f)

#define solSet_count(s) solHash_count(s->hash)
#define solSet_add(s, v) solHash_put_key_and_val(s->hash, v, SolNil)
#define solSet_in_set(s, v) solHash_has_key(s->hash, v)
#define solSet_is_empty(s) (solSet_count(s) == 0)
#define solSet_is_not_empty(s) (solSet_count(s) > 0)
#define solSet_del(s, v) solHash_remove(s->hash, v);

void* solSet_get(SolSet*);
int solSet_is_subset(SolSet*, SolSet*);
int solSet_has_intersection(SolSet*, SolSet*);
int solSet_equal(SolSet*, SolSet*);
int solSet_merge(SolSet*, SolSet*);
void* solSet_get_value_of_intersection(SolSet*, SolSet*);
SolSet* solSet_get_intersection(SolSet*, SolSet*);

#define solSet_rewind(s) solHashIter_rewind(s->iter)
#define solSet_next(s) solHashIter_next(s->iter)
#define solSetIter_current_count(s) s->iter->c
void* solSet_current(SolSet*);

void solSet_wipe(SolSet*);
int solSet_dup(SolSet*, SolSet*);

#endif

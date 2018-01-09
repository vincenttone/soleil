#ifndef _SOL_HASH_H_
#define _SOL_HASH_H_ 1
#include <stddef.h>
#include "sol_common.h"

#define SOL_HASH_INIT_SIZE 8
#define SOL_HASH_RESIZE_MAX_LOOP 100

#define SOL_HASH_RESIZING_Y 1
#define SOL_HASH_RESIZING_N 0

#define solHash_record_at_offset(r, o) (SolHashRecord*)(r + o)
#define solHash_grow(h) solHash_resize(h, h->size * 2)
#define solHash_add_record_extend(r) //

typedef struct _SolHashRecord {
    void *k;
    void *v;
} SolHashRecord;

typedef struct _SolHash {
    size_t size;
    size_t count;
    size_t mask;
    SolHashRecord *records;
    sol_f_hash_ptr f_hash1;
    sol_f_hash_ptr f_hash2;
    sol_f_cmp_ptr f_match;
    sol_f_dup_ptr f_dup_k;
    sol_f_dup_ptr f_dup_v;
    sol_f_free_ptr f_free_k;
    sol_f_free_ptr f_free_v;
    int is_resizing;
} SolHash;

typedef struct _SolHashIter {
    SolHash *hash;
    SolHashRecord *record;
    size_t c;
} SolHashIter;

SolHash* solHash_new();
SolHash* solHash_new_with_size(size_t);
void solHash_free(SolHash*);
int solHash_set_size(SolHash*, size_t);
int solHash_try_to_put(SolHash*, void*, void*);
int solHash_resize(SolHash*, size_t);
void solHash_wipe(SolHash*);
int solHash_dup(SolHash*, SolHash*);
SolHashRecord* solHash_find_record_by_key(SolHash*, void *);

#define solHash_size(h) h->size
#define solHash_count(h) h->count
#define solHash_is_empty(h) solHash_count(h) == 0
#define solHash_is_not_empty(h) solHash_count(h) != 0
#define solHash_update_mask(h) h->mask = h->size - 1

#define solHash_put(h, k, v) solHash_put_key_and_val(h, k, v)
#define solHash_get(h, k) solHash_find_value(h, k)

int solHash_put_key_and_val(SolHash*, void*, void*);
int solHash_has_key(SolHash*, void*);
void* solHash_find_value(SolHash*, void*);
int solHash_merge(SolHash*, SolHash*);
void solHash_remove(SolHash*, void*);

SolHashIter* solHashIter_new(SolHash*);
void solHashIter_free(SolHashIter*);
void solHashIter_rewind(SolHashIter*);
SolHashRecord* solHashIter_current_record(SolHashIter *iter);
void solHashIter_next(SolHashIter*);
SolHashRecord* solHashIter_get(SolHashIter *iter);
#define solHashIter_reset_count(i) i->c = 1

#define solHash_set_hash_func1(h, f) h->f_hash1 = f
#define solHash_set_hash_func2(h, f) h->f_hash2 = f
#define solHash_set_equal_func(h, f) h->f_match = f
#define solHash_set_free_k_func(h, f) h->f_free_k = f
#define solHash_set_free_v_func(h, f) h->f_free_v = f
#define solHash_set_dup_k_func(h, f) h->f_dup_k = f
#define solHash_set_dup_v_func(h, f) h->f_dup_v = f

#define solHash_hash_func1(h) h->f_hash1
#define solHash_hash_func2(h) h->f_hash2
#define solHash_equal_func(h) h->f_match
#define solHash_free_k_func(h) h->f_free_k
#define solHash_free_v_func(h) h->f_free_v

#define solHash_hash1(h, k) (*h->f_hash1)(k)
#define solHash_hash2(h, k) (*h->f_hash2)(k)
#define solHash_match(h, k1, k2) (*h->f_match)(k1, k2)
#define solHash_dup_k(h, k) (*h->f_dup_k)(k)
#define solHash_dup_v(h, v) (*h->f_dup_v)(v)
#define solHash_free_k(h, k) (*h->f_free_k)(k)
#define solHash_free_v(h, v) (*h->f_free_v)(v)

#endif

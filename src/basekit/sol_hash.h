#ifndef _SOL_HASH_H_
#define _SOL_HASH_H_ 1
#include <stddef.h>
#include "sol_common.h"

#define SOL_HASH_INIT_SIZE 8
#define SOL_HASH_RESIZE_MAX_LOOP 100

#define SOL_HASH_record_at_offset(r, o) (SolHashRecord*)(r + o)
#define SOL_HASH_grow(h) sol_hash_resize(h, h->size * 2)
#define SOL_HASH_record_extend(r) //

typedef size_t (*SolHashFunc)(void*);
typedef int (*SolHashEqualFunc)(void*, void*);

typedef struct SolHashRecord {
	void *k;
	void *v;
} SolHashRecord;

typedef struct SolHash {
	SolHashRecord *records;
	size_t size;
	size_t count;
	size_t mask;
	SolHashFunc hash_func1;
	SolHashFunc hash_func2;
	SolHashEqualFunc equal_func;
	int is_resizing;
} SolHash;

typedef struct SolHashIter {
	SolHash *hash;
	SolHashRecord *record;
	size_t num;
} SolHashIter;

SolHash* sol_hash_new();
void sol_hash_free(SolHash*);
int sol_hash_set_size(SolHash*, size_t);
int sol_hash_try_to_put(SolHash*, void*, void*);
int sol_hash_resize(SolHash*, size_t);
SolHashRecord* sol_hash_find_record_by_key(SolHash*, void *);

#define sol_hash_size(h) h->size
#define sol_hash_count(h) h->count
#define sol_hash_update_mask(h) h->mask = h->size - 1

#define sol_hash_put(h, k, v) sol_hash_put_key_and_val(h, k, v)
#define sol_hash_get(h, k) sol_hash_find_value(h, k)

int sol_hash_put_key_and_val(SolHash*, void*, void*);
void sol_hash_remove_by_key(SolHash*, void*);
int sol_hash_has_key(SolHash*, void*);
void* sol_hash_find_value(SolHash*, void *);

SolHashIter* sol_hash_iter_new(SolHash*);
void sol_hash_iter_free(SolHashIter*);
void sol_hash_iter_rewind(SolHashIter*);
SolHashRecord* sol_hash_iter_current_record(SolHashIter *iter);
void sol_hash_iter_next(SolHashIter*);

#define sol_hash_set_hash_func1(h, f) h->hash_func1 = f
#define sol_hash_set_hash_func2(h, f) h->hash_func2 = f
#define sol_hash_set_equal_func(h, f) h->equal_func = f
#define sol_hash_hash_func1(h) h->hash_func1
#define sol_hash_hash_func2(h) h->hash_func2
#define sol_hash_equal_func(h) h->equal_func

inline void sol_hash_records_free(SolHashRecord*);
inline SolHashRecord* sol_hash_record1_of_key(SolHash *hash, void *k);
inline SolHashRecord* sol_hash_record2_of_key(SolHash *hash, void *k);
inline void sol_hash_record_switch(SolHashRecord *r1, SolHashRecord *r2);
inline int sol_hash_add_records(SolHash *hash, SolHashRecord *records, size_t size);

#endif

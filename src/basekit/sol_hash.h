#ifndef _SOL_HASH_H_
#define _SOL_HASH_H_ 1
#include <stddef.h>
#include <stdbool.h>
#include "sol_common.h"

#define SOL_HASH_INIT_SIZE 8
#define sol_hash_record_at_offset_ (SolHashRecord*)(h->records + o * sizeof(SolHashRecord))

typedef size_t (SolHashFunc)(void*);
typedef short (SolHashEqualFunc)(void*, void*);

typedef struct SolHashRecord {
	void *k;
	void *v;
} SolHashRecord;

typedef struct SolHash {
	SolHashRecord *records;
	size_t size;
	size_t count;
	size_t mask;
	SolHashFunc *hash_func1;
	SolHashFunc *hash_func2;
	SolHashEqualFunc *equal_func;
	bool is_resizing;
} SolHash;

SolHash* sol_hash_new();
void* sol_hash_free(SolHash*);
void sol_hash_records_free(SolHashRecord*);
void sol_hash_set_size(SolHash*, size_t);
void sol_hash_update_mask(SolHash*);

void sol_hash_add_(SolHash*, void*, void*);
size_t sol_hash_count(SolHash*);
bool sol_hash_has_key(SolHash*, void*);
void* sol_hash_find_value(SolHash*, void *);

void sol_hash_set_hash_func1(SolHash*, SolHashFunc*);
void sol_hash_set_hash_func2(SolHash*, SolHashFunc*);
void sol_hash_set_equal_func(SolHash*, SolHashEqualFunc*);

inline SolHashRecord* sol_hash_record2_at_(SolHash *hash, void *k);
inline SolHashRecord* sol_hash_record1_at_(SolHash *hash, void *k);

#end

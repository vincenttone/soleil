#include "sol_hash.h"

SolHash *sol_hash_new()
{
	SolHash *hash = sol_calloc(1, sizeof(SolHash));
	if (hash == NULL) {
		return NULL;
	}
	if (sol_hash_set_size(hash, SOL_HASH_INIT_SIZE)) {
		return NULL;
	}
	return hash;
}

void sol_hash_free(SolHash *hash)
{
	sol_hash_records_free(hash->records);
	sol_free(hash);
}

void sol_hash_records_free(SolHashRecord *records)
{
	sol_free(records);
}

int sol_hash_set_size(SolHash *hash, size_t size)
{
	hash->records = sol_calloc(1, size * sizeof(SolHashRecord));
	if (hash->records == NULL) {
		return 1;
	}
	hash->size = size;
	sol_hash_update_mask(hash);
	return 0;
}

void sol_hash_update_mask(SolHash *hash)
{
	hash->mask = hash->size -1;
}

void sol_hash_set_hash_func1(SolHash *hash, SolHashFunc f)
{
	hash->hash_func1 = f;
}

void sol_hash_set_hash_func2(SolHash *hash, SolHashFunc f)
{
	hash->hash_func2 = f;
}

void sol_hash_set_equal_func(SolHash *hash, SolHashEqualFunc f)
{
	hash->equal_func = f;
}

size_t sol_hash_count(SolHash *hash)
{
	return hash->count;
}

void* sol_hash_find_value(SolHash *hash, void *k)
{
	SolHashRecord *r1 = sol_hash_record1_of_key(hash, k);
	if (r1->k != 0x0 && hash->equal_func(k, r1->k) == 0) {
		return r1->v;
	}
	SolHashRecord *r2 = sol_hash_record2_of_key(hash, k);
	if (r2->k != 0x0 && hash->equal_func(k, r2->k) == 0) {
		return r1->v;
	}
	return NULL;
}

int sol_hash_has_key(SolHash *hash, void *k)
{
	return sol_hash_find_value(hash, k) != NULL;
}

int sol_hash_put_key_and_val(SolHash *hash, void *k, void *v)
{
	SolHashRecord* r;
	r = sol_hash_record1_of_key(hash, k);
	if (r->k == 0x0) {
		sol_hash_record_set_key_and_val(r, k, v);
		hash->count++;
		return 0;
	}
	if (hash->equal_func(k, r->k)) {
		r->v = v;
		return 0;
	}
	r = sol_hash_record2_of_key(hash, k);
	if (r->k == 0x0) {
		sol_hash_record_set_key_and_val(r, k, v);
		hash->count++;
		return 0;
	}
	if (hash->equal_func(k, r->k)) {
		r->v = v;
		return 0;
	}
	// no place to put
	// optimizing and resize
	return sol_hash_try_to_put(hash, k, v);
}

int sol_hash_try_to_put(SolHash *hash, void *k, void *v)
{
	int i = 0;
	SolHashRecord *r, *record;
	record->k = k;
	record->v = v;
	for (i; i < SOL_HASH_UPGRADE_LIMIT; i++) {
		r = sol_hash_record1_of_key(hash, k);
		sol_hash_record_switch(r, record);
		if (r->k == 0x0) {
			sol_hash_record_set_key_and_val(r, k, v);
			hash->count++;
			return 0;
		}
		r = sol_hash_record2_of_key(hash, r->k);
		sol_hash_record_switch(r, record);
		if (r->k == 0x0) {
			sol_hash_record_set_key_and_val(r, k, v);
			hash->count++;
			return 0;
		}
	}
	if (hash->is_resizing) {
		return 1;
	}
	if (sol_hash_grow(hash)) {
		return 1;
	}
	return sol_hash_put_key_and_val(hash, k, v);
}

int sol_hash_resize(SolHash *hash, size_t size)
{
	SolHashRecord *records = hash->records;
	size_t old_size = hash->size;
	int loop_limit = SOL_HASH_RESIZE_MAX_LOOP;
	hash->is_resizing = 1;
	do {
		hash->records = sol_calloc(1, size * sizeof(SolHashRecord));
		hash->count = 0;
		if (sol_hash_add_records(hash, records, old_size) == 0) {
			hash->is_resizing = 0;
		} else {
			size = size * 2;
			sol_free(hash->records);
		}
	} while (loop_limit-- && hash->is_resizing);
	if (hash->is_resizing) {
		hash->is_resizing = 0;
		hash->records = records;
		return 1;
	} else {
		sol_free(records);
		return 0;
	}
}

inline int sol_hash_add_records(SolHash *hash, SolHashRecord *records, size_t size)
{
	SolHashRecord *r;
	size_t offset = 0;
	for (offset; offset < size; offset++) {
		r = sol_hash_record_at_offset(r, offset);
		if (r->k) {
			if (sol_hash_put_key_and_val(hash, r->k, r->v)) {
				return 1;
			}
		}
	}
	return 0;
}

inline void sol_hash_record_switch(SolHashRecord *r1, SolHashRecord *r2)
{
	SolHashRecord tmp = *r1;
	*r1 = *r2;
	*r1 = tmp;
}

inline SolHashRecord* sol_hash_record1_of_key(SolHash *hash, void *k)
{
	size_t offset = (hash->hash_func1(k) & hash->mask);
	SolHashRecord *record = sol_hash_record_at_offset(hash->records, offset);
	return record;
}

inline SolHashRecord* sol_hash_record2_of_key(SolHash *hash, void *k)
{
	size_t offset = (hash->hash_func2(k) & hash->mask);
	SolHashRecord *record = sol_hash_record_at_offset(hash->records, offset);
	return record;
}

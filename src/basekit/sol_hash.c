#include "sol_hash.h"

SolHash* sol_hash_new()
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

inline void sol_hash_records_free(SolHashRecord *records)
{
	sol_free(records);
}

int sol_hash_set_size(SolHash *hash, size_t size)
{
	hash->records = sol_calloc(size, sizeof(SolHashRecord));
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
	SolHashRecord *r = sol_hash_record1_of_key(hash, k);
	if (r->k != NULL && hash->equal_func(k, r->k) == 0) {
		return r->v;
	}
	r = sol_hash_record2_of_key(hash, k);
	if (r->k != NULL && hash->equal_func(k, r->k) == 0) {
		return r->v;
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
	if (r->k == NULL) {
		SOL_HASH_record_extend(r);
		r->k = k;
		r->v = v;
		hash->count++;
		return 0;
	}
	if (hash->equal_func(k, r->k) == 0) {
		r->v = v;
		return 0;
	}
	r = sol_hash_record2_of_key(hash, k);
	if (r->k == NULL) {
		SOL_HASH_record_extend(r);
		r->k = k;
		r->v = v;
		hash->count++;
		return 0;
	}
	if (hash->equal_func(k, r->k) == 0) {
		r->v = v;
		return 0;
	}
	// no place to put
	// adjust and resize
	return sol_hash_try_to_put(hash, k, v);
}

int sol_hash_try_to_put(SolHash *hash, void *k, void *v)
{
	int i = 0;
	SolHashRecord *r, rs;
	rs.k = k;
	rs.v = v;
	for (i; i < SOL_HASH_UPGRADE_LIMIT; i++) {
		// get conflict hash record
		r = sol_hash_record1_of_key(hash, k);
		// try to put record
		if (r->k == NULL) {
			SOL_HASH_record_extend(r);
			r->k = k;
			r->v = v;
			hash->count++;
			return 0;
		}
		// conflict exists
		// switch rs -> record, conflict record switched to rs
		sol_hash_record_switch(r, &rs);
		// second hash step
		r = sol_hash_record2_of_key(hash, rs.k);
		if (r->k == NULL) {
			SOL_HASH_record_extend(r);
			r->k = rs.k;
			r->v = rs.v;
			hash->count++;
			return 0;
		}
		sol_hash_record_switch(r, &rs);
	}
	if (hash->is_resizing) {
		return 1;
	}
	if (SOL_HASH_grow(hash)) {
		return 1;
	}
	return sol_hash_put_key_and_val(hash, rs.k, rs.v);
}

int sol_hash_resize(SolHash *hash, size_t size)
{
	SolHashRecord *records = hash->records;
	size_t old_size = hash->size;
	int loop_limit = SOL_HASH_RESIZE_MAX_LOOP;
	hash->is_resizing = 1;
	do {
		if (sol_hash_set_size(hash, size)) {
			hash->records = records;
			hash->is_resizing = 0;
			return 1;
		}
		hash->count = 0;
		if (sol_hash_add_records(hash, records, old_size) == 0) {
			hash->is_resizing = 0;
		} else {
			size = size * 2;
			sol_free(hash->records);
		}
	} while (loop_limit-- && hash->is_resizing);
	if (hash->is_resizing) {
		hash->records = records;
		hash->is_resizing = 0;
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
		r = SOL_HASH_record_at_offset(records, offset);
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
	*r2 = tmp;
}

inline SolHashRecord* sol_hash_record1_of_key(SolHash *hash, void *k)
{
	size_t offset = (hash->hash_func1(k) & hash->mask);
	return SOL_HASH_record_at_offset(hash->records, offset);
}

inline SolHashRecord* sol_hash_record2_of_key(SolHash *hash, void *k)
{
	size_t offset = (hash->hash_func2(k) & hash->mask);
	SolHashRecord *record = SOL_HASH_record_at_offset(hash->records, offset);
	return record;
}

SolHashIter* sol_hash_iter_new(SolHash *hash)
{
	SolHashIter *iter = sol_alloc(sizeof(SolHashIter));
	if (iter == NULL) {
		return NULL;
	}
	iter->hash = hash;
	sol_hash_iter_rewind(iter);
	return iter;
}

void sol_hash_iter_free(SolHashIter *iter)
{
	sol_free(iter);
}

SolHashRecord* sol_hash_iter_current_record(SolHashIter *iter)
{
	return iter->record;
}

void sol_hash_iter_rewind(SolHashIter *iter)
{
	iter->record = iter->hash->records;
	iter->num = 1;
}

SolHashRecord* sol_hash_iter_next(SolHashIter *iter)
{
	if (iter->num++ < iter->hash->size) {
		iter->record++;
		if (iter->record->k) {
			return iter->record;
		}
		return NULL;
	}
	return NULL;
}

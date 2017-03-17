#include "sol_hash.h"

SolHash *sol_hash_new()
{
	SolHash *hash = sol_calloc(1, sizeof(SolHash));
	sol_hash_set_size(*hash, SOL_HASH_INIT_SIZE);
	return *hash;
}

void sol_hash_free(SolHash *hash)
{
	sol_hash_records_free(hash->records);
	sol_free(hash);
}

void sol_hash_records_free(SolHashRecords *records)
{
	sol_free(records);
}

void sol_hash_set_size(SolHash *hash, size_t size)
{
	hash->records = sol_realloc(hash->records, size * sizeof(SolHashRecord));
	if (size > hash->size) {
		memset(hash->records + size * sizeof(SolHashRecord),
			   0x0,
			   (size - hash->size) * sizeof(SolHashRecord)
			   );
	}
	hash->size = size;
	sol_hash_update_mask(hash);
}

void sol_hash_update_mask(SolHash *hash)
{
	hash->mask = hash->size -1;
}

void sol_hash_set_hash_func1(SolHash *hash, SolHashFunc *f)
{
	hash->hash_func1 = f;
}

void sol_hash_set_hash_func2(SolHash *hash, SolHashFunc *f)
{
	hash->hash_func2 = f;
}

void sol_hash_set_equal_func(SolHash *hash, SolHashEqualFunc *f)
{
	hash->equal_func = f;
}

size_t sol_hash_count(SolHash *hash)
{
	return hash->count;
}

void* sol_hash_find_value(SolHash *hash, void *k)
{
	SolHashRecord *r1 = sol_hash_record1_at_(hash, k);
	if (r1->k != 0x0 && hash->equal_func(k, r1->k)) {
		return r1->v;
	}
	SolHashRecord *r2 = sol_hash_record2_at_(hash, k);
	if (r2->k != 0x0 && hash->equal_func(k, r2->k)) {
		return r1->v;
	}
	return NULL;
}

bool sol_hash_has_key(SolHash *hash, void *k)
{
	return sol_hash_find_value(hash, k) != NULL;
}

void sol_hash_add_(SolHash *hash, void *k, void *v)
{
	SolHashRecord* r;
	r = sol_hash_record1_at(hash, k);
	if (r->k == 0x0) {
		r->k = k;
		r->v = v;
		hash->count++;
		return;
	}
	if (hash->equal_func(k, hash->k)) {
		r->v = v;
		return;
	}
	r = sol_hash_record2_at(hash, k);
	if (r->k == 0x0) {
		r->k = k;
		r->v = v;
		hash->count++;
		return;
	}
	if (hash->equal_func(k, hash->k)) {
		r->v = v;
		return;
	}
	// todo no place to put
	// optimizing and resize
}

inline SolHashRecord* sol_hash_record1_at_(SolHash *hash, void *k)
{
	size_t offset = (hash->hash_func1(k) & hash->mask);
	SolHashRecord *record = sol_hash_record_at_offset_(hash, offset);
	return record;
}

inline SolHashRecord* sol_hash_record2_at_(SolHash *hash, void *k)
{
	size_t offset = (hash->hash_func2(k) & hash->mask);
	SolHashRecord *record = sol_hash_record_at_offset_(hash, offset);
	return record;
}

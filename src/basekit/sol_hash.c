#include <string.h>
#include <assert.h>
#include "sol_hash.h"

static inline void solHash_free_records(SolHashRecord *r, size_t s, sol_f_free_ptr fk, sol_f_free_ptr fv)
{
    SolHashRecord *cr;
    if (fk || fv) {
        size_t o = 0;
        while(o < s) {
            cr = solHash_record_at_offset(r, o);
            if (fk && cr->k) {
                (*fk)(cr->k);
            }
            if (fv && cr->v) {
                (*fv)(cr->v);
            }
            o++;
        }
    }
    sol_free(r);
}

static inline int solHash_add_records(SolHash *hash, SolHashRecord *records, size_t size)
{
    SolHashRecord *r;
    size_t offset = 0;
    while(offset < size) {
        r = solHash_record_at_offset(records, offset);
        if (r->k) {
            if (solHash_put_key_and_val(hash, r->k, r->v)) {
                return 5;
            }
        }
        offset++;
    }
    return 0;
}

static inline void solHash_record_switch(SolHashRecord *r1, SolHashRecord *r2)
{
    SolHashRecord tmp = *r1;
    *r1 = *r2;
    *r2 = tmp;
}

static inline SolHashRecord* solHash_record1_of_key(SolHash *hash, void *k)
{
    size_t offset = (solHash_hash1(hash, k) & hash->mask);
    return solHash_record_at_offset(hash->records, offset);
}

static inline SolHashRecord* solHash_record2_of_key(SolHash *hash, void *k)
{
    size_t offset = (solHash_hash2(hash, k) & hash->mask);
    SolHashRecord *record = solHash_record_at_offset(hash->records, offset);
    return record;
}

SolHash* solHash_new()
{
    SolHash *hash = sol_calloc(1, sizeof(SolHash));
    if (hash == NULL) {
        return NULL;
    }
    if (solHash_set_size(hash, SOL_HASH_INIT_SIZE)) {
        return NULL;
    }
    return hash;
}

void solHash_free(SolHash *hash)
{
    solHash_free_records(hash->records, hash->size, hash->f_free_k, hash->f_free_v);
    sol_free(hash);
}

int solHash_set_size(SolHash *hash, size_t size)
{
    hash->records = sol_calloc(size, sizeof(SolHashRecord));
    if (hash->records == NULL) {
        return 8;
    }
    hash->size = size;
    solHash_update_mask(hash);
    return 0;
}

void solHash_wipe(SolHash *hash)
{
    memset(hash->records, 0x0, sizeof(SolHashRecord) * hash->size);
    hash->count = 0;
}

int solHash_dup(SolHash *h1, SolHash *h2)
{
    if (h1->size != h2->size) {
        solHash_free_records(h1->records, h1->size, h1->f_free_k, h1->f_free_v);
        if (solHash_set_size(h1, h2->size) != 0) {
            return 1;
        }
    }
    SolHashRecord *r = h1->records;
    memcpy(h1, h2, sizeof(SolHash));
    h1->records = r;
    if (h1->f_dup_k || h1->f_dup_v) {
        size_t offset = 0;
        void *k;
        void *v;
        while(offset < h2->size) {
            r = solHash_record_at_offset(h2->records, offset);
            if (r->k) {
                if (h1->f_dup_k) {
                    k = solHash_dup_k(h1, r->k);
                } else {
                    k = r->k;
                }
                if (h1->f_dup_v) {
                    v = solHash_dup_v(h1, r->v);
                } else {
                    v = r->v;
                }
                if (solHash_put_key_and_val(h1, k, v)) {
                    return 5;
                }
            }
            offset++;
        }
    } else {
        memcpy(h1->records, h2->records, sizeof(SolHashRecord) * h1->size);
    }
    return 0;
}

SolHashRecord* solHash_find_record_by_key(SolHash *hash, void *k)
{
    assert(solHash_hash_func1(hash) && "no hash func (1)");
    assert(solHash_hash_func2(hash) && "no hash func (2)");
    assert(solHash_equal_func(hash) && "no match func");
    SolHashRecord *r = solHash_record1_of_key(hash, k);
    if (r->k != NULL && solHash_match(hash, k, r->k) == 0) {
        return r;
    }
    r = solHash_record2_of_key(hash, k);
    if (r->k != NULL && solHash_match(hash, k, r->k) == 0) {
        return r;
    }
    return NULL;
}

void solHash_remove(SolHash *hash, void *k)
{
    assert(solHash_hash_func1(hash) && "no hash func (1)");
    assert(solHash_hash_func2(hash) && "no hash func (2)");
    assert(solHash_equal_func(hash) && "no match func");
    SolHashRecord *r = solHash_record1_of_key(hash, k);
    if (r->k != NULL && solHash_match(hash, k, r->k) == 0) {
        goto remove_record;
    }
    r = solHash_record2_of_key(hash, k);
    if (r->k != NULL && solHash_match(hash, k, r->k) == 0) {
        goto remove_record;
    }
    goto finish;
 remove_record:
    if (solHash_free_k_func(hash)) {
        solHash_free_k(hash, r->k);
    }
    if (solHash_free_v_func(hash)) {
        solHash_free_v(hash, r->v);
    }
    memset(r, 0x0, sizeof(SolHashRecord));
 finish:
    return;
}

void* solHash_find_value(SolHash *hash, void *k)
{
    SolHashRecord *r = solHash_find_record_by_key(hash, k);
    if (r == NULL) {
        return NULL;
    }
    return r->v;
}

int solHash_has_key(SolHash *hash, void *k)
{
    if (solHash_find_record_by_key(hash, k)) {
        return 0;
    }
    return 1;
}

int solHash_put_key_and_val(SolHash *hash, void *k, void *v)
{
    assert(solHash_hash_func1(hash) && "no hash func (1)");
    assert(solHash_hash_func2(hash) && "no hash func (2)");
    assert(solHash_equal_func(hash) && "no match func");
    SolHashRecord* r;
    r = solHash_record1_of_key(hash, k);
    if (r->k == NULL) {
        solHash_add_record_extend(r);
        r->k = k;
        r->v = v;
        hash->count++;
        return 0;
    }
    if (solHash_match(hash, k, r->k) == 0) {
        solHash_add_record_extend(r);
        r->v = v;
        return 0;
    }
    r = solHash_record2_of_key(hash, k);
    if (r->k == NULL) {
        solHash_add_record_extend(r);
        r->k = k;
        r->v = v;
        hash->count++;
        return 0;
    }
    if (solHash_match(hash, k, r->k) == 0) {
        solHash_add_record_extend(r);
        r->v = v;
        return 0;
    }
    // no place to put
    // adjust and resize
    return solHash_try_to_put(hash, k, v);
}

int solHash_try_to_put(SolHash *hash, void *k, void *v)
{
    SolHashRecord *r;
    SolHashRecord rs;
    rs.k = k;
    rs.v = v;
    size_t i = 0;
    for (; i < hash->size * 2; i++) {
        // get conflict hash record
        r = solHash_record1_of_key(hash, k);
        // try to put record
        if (r->k == NULL) {
            solHash_add_record_extend(r);
            r->k = k;
            r->v = v;
            hash->count++;
            return 0;
        }
        // conflict exists
        // switch rs -> record, conflict record switched to rs
        solHash_record_switch(r, &rs);
        // second hash step
        r = solHash_record2_of_key(hash, rs.k);
        if (r->k == NULL) {
            solHash_add_record_extend(r);
            r->k = rs.k;
            r->v = rs.v;
            hash->count++;
            return 0;
        }
        solHash_record_switch(r, &rs);
    }
    if (hash->is_resizing == SOL_HASH_RESIZING_Y) {
        return 2;
    }
    if (solHash_grow(hash)) {
        return 3;
    }
    return solHash_put_key_and_val(hash, rs.k, rs.v);
}

int solHash_resize(SolHash *hash, size_t size)
{
    SolHashRecord *records = hash->records;
    size_t old_size = hash->size;
    int loop_limit = SOL_HASH_RESIZE_MAX_LOOP;
    hash->is_resizing = SOL_HASH_RESIZING_Y;
    do {
        if (solHash_set_size(hash, size) != 0) {
            hash->records = records;
            hash->size = old_size;
            hash->is_resizing = SOL_HASH_RESIZING_N;
            return 6;
        }
        hash->count = 0;
        if (solHash_add_records(hash, records, old_size) == 0) {
            hash->is_resizing = SOL_HASH_RESIZING_N;
        } else {
            size = size * 2;
            solHash_free_records(hash->records, hash->size, NULL, NULL);
        }
    } while (loop_limit-- && hash->is_resizing == SOL_HASH_RESIZING_Y);
    if (hash->is_resizing == SOL_HASH_RESIZING_Y) {
        hash->records = records;
        hash->size = old_size;
        hash->is_resizing = SOL_HASH_RESIZING_N;
        return 7;
    } else {
        solHash_free_records(records, old_size, NULL, NULL);
        return 0;
    }
}

int solHash_merge(SolHash *h1, SolHash *h2)
{
    if (h2 == NULL) {
        return 0;
    }
    if (h1 == NULL) {
        h1 = h2;
        return 0;
    }
    return solHash_add_records(h1, h2->records, solHash_size(h2));
}

SolHashIter* solHashIter_new(SolHash *hash)
{
    SolHashIter *iter = sol_alloc(sizeof(SolHashIter));
    if (iter == NULL) {
        return NULL;
    }
    iter->hash = hash;
    solHashIter_rewind(iter);
    return iter;
}

void solHashIter_free(SolHashIter *iter)
{
    sol_free(iter);
}

SolHashRecord* solHashIter_current_record(SolHashIter *iter)
{
    if (iter->record->k) {
        return iter->record;
    }
    return NULL;
}

void solHashIter_rewind(SolHashIter *iter)
{
    iter->record = iter->hash->records;
    solHashIter_reset_count(iter);
}

void solHashIter_next(SolHashIter *iter)
{
    if (iter->c < iter->hash->size) {
        iter->record++;
        iter->c++;
    } else if (iter->c == iter->hash->size) {
        iter->c++;
    }
}

SolHashRecord* solHashIter_get(SolHashIter *iter)
{
    SolHashRecord *r;
    while (iter->c <= solHash_size(iter->hash)) {
        r = solHashIter_current_record(iter);
        solHashIter_next(iter);
        if (r && r->k) {
            return r;
        }
    }
    return NULL;
}

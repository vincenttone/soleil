#include <assert.h>
#include "sol_triple.h"

SolTriple* solTriple_new()
{
    SolTriple *t = sol_calloc(1, sizeof(SolTriple));
    if (t == NULL) {
        return NULL;
    }
    if (solTriple_set_size(t, SOL_TRIPLE_INIT_SIZE) != 0) {
        return NULL;
    }
    return t;
}

int solTriple_set_size(SolTriple *t, size_t s)
{
    SolTripleRecord *r = sol_calloc(1, sizeof(SolTripleRecord) * s);
    if (r == NULL) {
        return -1;
    }
    solTriple_set_records(t, r);
    solTriple_set_size(t, s);
    solTriple_update_mask(t);
    return 0;
}

void solTriple_free(SolTriple *t)
{
    if (solTriple_record(t)) {
        solTripleRecord_free(solTriple_record(t));
    }
    sol_free(t);
}

int solTripleRecord_register(SolTriple *t, SolTripleRecord *r)
{
    //solHash_put(solTripleRecord_v1(r1), r);
}

int solTriple_add(SolTriple *t, void *v1, void *v2, void *v3)
{
    SolTripleRecord * r = solTriple_record1_of_val(t, v1, v2);
    if (solTripleRecord_v1(r) == NULL) {
        goto up_record;
    }
    if (solTriple_v1_match(solTripleRecord_v1(r), v1) == 0
        && solTriple_v2_match(solTripleRecord_v2(r), v2) == 0
        ) {
        solTripleRecord_set_v3(r, v3);
        return 0;
    }
    r = solTriple_record2_of_val(t, v1, v2);
    if (solTripleRecord_v1(r) == NULL) {
        goto up_record;
    }
    if (solTriple_v1_match(solTripleRecord_v1(r), v1) == 0
        && solTriple_v2_match(solTripleRecord_v2(r), v2) == 0
        ) {
        solTripleRecord_set_v3(r, v3);
        return 0;
    }
    return solTriple_adjust_and_put(t, v1, v2, v3);
up_record:
    if (solTripleRecord_register(t, r) != 0) {
        solTripleRecord_set_v1(r, NULL);
        return -1;
    }
    solTripleRecord_set_v1(r, v1);
    solTripleRecord_set_v2(r, v2);
    solTripleRecord_set_v3(r, v3);
    solTriple_count_incr(t);
    return 0;
}

int solTriple_adjust_and_put(SolTriple *t, void *v1, void *v2, void *v3)
{
    SolTripleRecord *r;
    SolTripleRecord rs;
    size_t i;
    for (i = 0; i < solTriple_size(t) * 2; i++) {
        r = solTriple_record1_of_val(t, v1, v2);
        if (solTripleRecord_v1(r) == NULL) {
            goto up_record;
        }
        solTriple_record_switch(r, &rs);
        r = solTriple_record2_of_val(t, v1, v2);
        if (solTripleRecord_v1(r) == NULL) {
            goto up_record;
        }
        solTriple_record_switch(r, &rs);
    }
    if (solTriple_is_resizing(t)) {
        return -2;
    }
    if (solTriple_grow(t) != 0) {
        return -3;
    }
    return solTriple_add(t, v1, v2, v3);
up_record:
    if (solTripleRecord_register(t, r) != 0) {
        solTripleRecord_set_v1(r, NULL);
        return -1;
    }
    solTripleRecord_set_v1(r, v1);
    solTripleRecord_set_v2(r, v2);
    solTripleRecord_set_v3(r, v3);
    solTriple_count_incr(t);
    return 0;
}

int solTriple_resize(SolTriple *t, size_t s)
{
    SolTripleRecord *r = solTriple_records(t);
    size_t *os = solTriple_size(t);
    size_t l = SOL_TRIPLE_RESIZE_LIMIT;
    solTriple_set_start_resizing_flag(t);
    while (l-- && solTriple_is_resizing(t)) {
        if (solTriple_set_size(t, s) != 0) {
            solTriple_set_records(t, r);
            // fix record node
            solTriple_set_size(t, os);
            solTriple_set_finish_resizing_flag(t);
            return -1;
        }
        solTriple_set_count(t, 0);
        if (solTriple_add_records(t, r, os) == 0) {
            solTriple_set_finish_resizing_flag(t);
        } else {
            s = s * 2;
            solTriple_free_records(solTriple_records(t), solTriple_size(t));
        }
    }
    if (solTriple_is_resizing(t)) {
        solTriple_set_records(t, r);
        // fix record node
        solTriple_set_size(t, os);
        solTriple_set_finish_resizing_flag(t);
        return -2;
    }
    solTriple_free_records(r, os);
    return 0;
}

inline int solTriple_add_records(SolTriple *t, SolTripleRecord *records, size_t size)
{
    SolTripleRecord *r;
    size_t offset = 0;
    while(offset < size) {
        r = solTriple_record_at_offset(records, offset);
        if (solTripleRecord_v1(r)) {
            if (solTriple_add(t, solTripleRecord_v1(r), solTripleRecord_v2(r), solTripleRecord_v3(r)) != 0) {
                return -1;
            }
        }
        offset++;
    }
    return 0;
}

inline SolHashRecord* solTriple_record1_of_val(SolTriple *t, void *v1, void *v2)
{
    size_t o1 = solTriple_v1_hash1(t, v1);
    size_t o2 = solTriple_v2_hash1(t, v2);
    o = (o1 & solTriple_mask(t) + o2 & solTriple_mask(t)) & solTriple_mask(t);
    return solTriple_record_at_offset(solTriple_records(t), o);
}

inline SolHashRecord* solTriple_record2_of_val(SolTriple *t, void *v1, void *v2)
{
    size_t o1 = solTriple_v1_hash2(t, v1);
    size_t o2 = solTriple_v2_hash2(t, v2);
    o = (o1 & solTriple_mask(t) + o2 & solTriple_mask(t)) & solTriple_mask(t);
    return solTriple_record_at_offset(solTriple_records(t), o);
}

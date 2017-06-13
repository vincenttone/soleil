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
    solTriple_set_record(t, r);
    solTriple_set_size(t, s);
    solTriple_update_mask(t);
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
        solTripleRecord_set_v1(r, v1);
        solTripleRecord_set_v2(r, v2);
        solTripleRecord_set_v3(r, v3);
        if (solTripleRecord_register(t, r) != 0) {
            solTripleRecord_set_v1(r, NULL);
            return -1;
        }
        return 0;
    }
    r = solTriple_record2_of_val(t, v1, v2);
    if (solTripleRecord_v1(r) == NULL) {
        solTripleRecord_set_v1(r, v1);
        solTripleRecord_set_v2(r, v2);
        solTripleRecord_set_v3(r, v3);
        if (solTripleRecord_register(t, r) != 0) {
            solTripleRecord_set_v1(r, NULL);
            return -1;
        }
        return 0;
    }
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
    size_t o = solTriple_v1_hash1(t, v1) + solTriple_v2_hash1(t, v2);
    o = o & solTriple_mask(t);
    return solTriple_record_at_offset(solTriple_records(t), o);
}

inline SolHashRecord* solTriple_record2_of_val(SolTriple *t, void *v1, void *v2)
{
    size_t o = solTriple_v1_hash2(t, v1) + solTriple_v2_hash2(t, v2);
    o = o & solTriple_mask(t);
    return solTriple_record_at_offset(solTriple_records(t), o);
}

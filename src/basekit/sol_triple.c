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

int solTriple_add(SolTriple *t, void *v1, void *v2, void *v3)
{
    
}

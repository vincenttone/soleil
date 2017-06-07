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

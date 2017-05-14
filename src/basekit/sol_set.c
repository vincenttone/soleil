#include "sol_set.h"

SolSet* solSet_new()
{
    SolHash *hash = solHash_new();
    SolHashIter *iter = solHashIter_new(hash);
    SolSet *s = sol_alloc(sizeof(SolSet));
    s->hash = hash;
    s->iter = iter;
    return s;
}

void solSet_free(SolSet *s)
{
    solHashIter_free(s->iter);
    solHash_free(s->hash);
    sol_free(s);
}

inline void* solSet_current(SolSet *s)
{
    SolHashRecord *r = solHashIter_current_record(s->iter);
    if (r == NULL) {
        return NULL;
    }
    return r->k;
}

void* solSet_get(SolSet *s)
{
    SolHashRecord *r = solHashIter_get(s->iter);
    if (r == NULL) {
        return NULL;
    }
    return r->k;
}

int solSet_is_subset(SolSet *s1, SolSet *s2)
{
    void *v;
    solSet_rewind(s2);
    while ((v = solSet_get(s2))) {
        if (solSet_in_set(s1, v) == 1) {
            return 1;
        }
    }
    return 0;
}

int solSet_has_intersection(SolSet *s1, SolSet *s2)
{
    void *v;
    solSet_rewind(s1);
    while ((v = solSet_get(s1))) {
        if (solSet_in_set(s2, v) == 0) {
            return 0;
        }
    }
    return 1;
}
/**
 * get intersection values
 * need rewind s1 by yourself
 * return one value by invoking once
 */
void* solSet_get_value_of_intersection(SolSet *s1, SolSet *s2)
{
    void *v;
    while ((v = solSet_get(s1))) {
        if (solSet_in_set(s2, v) == 0) {
            return v;
        }
    }
    return NULL;
}

SolSet* solSet_get_intersection(SolSet *s1, SolSet *s2)
{
    SolSet *s = solSet_new();
    solSet_set_hash_func1(s, solSet_hash_func1(s1));
    solSet_set_hash_func2(s, solSet_hash_func2(s1));
    solSet_set_equal_func(s, solSet_equal_func(s1));
    solSet_set_free_func(s, solSet_free_func(s1));
    solSet_rewind(s1);
    void *v;
    while ((v = solSet_get_value_of_intersection(s1, s2))) {
        solSet_add(s, v);
    }
    return s;
}

int solSet_equal(SolSet *s1, SolSet *s2)
{
    if (solSet_count(s1) != solSet_count(s2)) {
        return 1;
    }
    return solSet_is_subset(s1, s2);
}

int solSet_merge(SolSet *s, SolSet *s1)
{
    if (s1 == NULL) {
        return 0;
    }
    if (s == NULL) {
        s = s1;
        return 0;
    }
    int rtn = 0;
    void *v;
    solSet_rewind(s1);
    while ((v = solSet_get(s1))) {
        rtn = solSet_add(s, v);
        if (rtn != 0) {
            return rtn;
        }
    }
    return 0;
}

void solSet_wipe(SolSet *s)
{
    solHash_wipe(s->hash);
    solHashIter_rewind(s->iter);
}

int solSet_dup(SolSet *s1, SolSet *s2)
{
    if (solHash_dup(s1->hash, s2->hash) != 0) {
        return 1;
    }
    solHashIter_rewind(s1->iter);
    return 0;
}

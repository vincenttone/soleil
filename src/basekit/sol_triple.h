#ifndef _SOL_TRIPLE_H_
#define _SOL_TRIPLE_H_ 1

#include "sol_common.h"
#include "sol_hash.h"
#include "sol_list.h"

#define SOL_TRIPLE_INIT_SIZE 8
#define SOL_TRIPLE_RESIZE_LIMIT 64

typedef struct _SolTripleRecord {
    struct v1 {
        void *v;
        SolListNode *n;
    };
    void *v2;
    void *v3;
} SolTripleRecord;

typedef struct _SolTriple {
    size_t mask;
    size_t size;
    size_t count;
    int flag;
    SolTripleRecord *r;
    SolHash *m;
    sol_f_hash_ptr f_v1_hash1;
    sol_f_hash_ptr f_v1_hash2;
    sol_f_hash_ptr f_v2_hash1;
    sol_f_hash_ptr f_v2_hash2;
    sol_f_match_ptr f_v1_match;
    sol_f_match_ptr f_v2_match;
} SolTriple;

SolTriple* solTriple_new();
void solTriple_free(SolTriple*);
int solTriple_add(SolTriple*, void*, void*, void*);
void* solTriple_get_by_v1(SolTriple*, void*);
void* solTriple_get_v3(SolTriple*, void*, void*);
int solTriple_remove_by_v1(SolTriple*, void*);
int solTriple_remove_v3(SolTriple*, void*, void*);
int solTriple_set_size(SolTriple*, size_t);

SolTripleRecord* solTripleRecord_new();
void solTripleRecord_free();

#define solTriple_set_records(t, r) t->record = r
#define solTriple_set_size(t, s) t->size = s
#define solTriple_set_count(t, c) t->count = c
#define solTriple_update_mask(t) t->mask = t->size - 1

#define solTriple_records(t) t->record
#define solTriple_size(t) t->size
#define solTriple_mask(t) t->mask
#define solTriple_count(t) t->count

#define solTriple_count_incr(t) t->count++

#define solTripleRecord_set_v1(r, val) r->v1->v = val
#define solTripleRecord_set_v2(r, val) r->v2 = val
#define solTripleRecord_set_v3(r, val) r->v3 = val

#define solTripleRecord_v1(r) r->v1->v
#define solTripleRecord_v2(r) r->v2
#define solTripleRecord_v3(r) r->v3

#define solTripleRecord_set_v1_node(r, node) r->v1->n = node
#define solTripleRecord_v1_node(r) r->v1->n

#define solTriple_v1_hash1(h) (*h->f_v1_hash1)
#define solTriple_v1_hash2(h) (*h->f_v1_hash2)
#define solTriple_v1_match(h) (*h->f_v1_match)

#define solTriple_v2_hash1(h) (*h->f_v2_hash1)
#define solTriple_v2_hash2(h) (*h->f_v2_hash2)
#define solTriple_v2_match(h) (*h->f_v2_match)

#define solTriple_record_at_offset(r, o) (SolTripleRecord*)(r + o)
#define solTriple_grow(t) solTriple_resize(t, solTriple_size(t) *2)
#define solTriple_is_resizing(t) t->flag & 0x1
#define solTriple_set_start_resizing_flag(t) t->is_resizing | 0x1
#define solTriple_set_finish_resizing_flag(t) t->is_resizing | 0x0

#endif

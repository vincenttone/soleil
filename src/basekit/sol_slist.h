#ifndef _SOL_SLIST_H_
#define _SOL_SLIST_H_ 1

#include "sol_common.h"

typedef struct _SolSlistNode {
    void *val;
    struct _SolSlistNode *next;
} SolSlistNode;

typedef struct _SolSlist {
    SolSlistNode *head;
    SolSlistNode *tail;
    unsigned long len;
    void *(*f_dup)(void*);
    void (*f_free)(void*);
    int (*f_match)(void*, void*);
} SolSlist;

#define solSlist_set_head(l, n) (l)->head = n
#define solSlist_set_tail(l, n) (l)->tail = n
#define solSlist_set_len(l, c) (l)->len = c
#define solSlist_incr_len(l) (l)->len++
#define solSlist_decr_len(l) (l)->len--

#define solSlist_head(l) (l)->head
#define solSlist_tail(l) (l)->tail
#define solSlist_len(l) (l)->len

#define solSlist_set_val_dup_func(l, f) (l)->f_dup = f
#define solSlist_set_val_free_func(l, f) (l)->f_free = f
#define solSlist_set_val_match_func(l, f) (l)->f_match = f

#define solSlistVal_free_func(l) (l)->f_free
#define solSlistVal_match_func(l) (l)->f_match

#define solSlistVal_dup(l, v) (*(l)->f_dup)(v)
#define solSlistVal_free(l, v) (*(l)->f_free)(v)
#define solSlistVal_match(l, v1, v2) (*(l)->f_match)(v1, v2)

#define solSlistNode_set_val(n, v) (n)->val = v
#define solSlistNode_set_next(n, node) (n)->next = node
#define solSlistNode_val(n) (n)->val
#define solSlistNode_next(n) (n)->next

SolSlist* solSlist_new();
void solSlist_free(SolSlist*);
SolSlistNode* solSlist_add(SolSlist*, void*);
int solSlist_del_node(SolSlist*, SolSlistNode*);
int solSlist_remove(SolSlist*, void*);
int solSlist_has(SolSlist*, void*);
int solSlist_merge(SolSlist*, SolSlist*);

SolSlistNode* solSlistNode_new();
void solSlistNode_free(SolSlist*, SolSlistNode*);

#endif

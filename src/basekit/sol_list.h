#ifndef _SOL_SLIST_H_
#define _SOL_SLIST_H_ 1

#include "sol_common.h"

typedef struct _SolListNode {
    void *val;
    struct _SolListNode *next;
} SolListNode;

typedef struct _SolList {
    SolListNode *head;
    SolListNode *tail;
    unsigned long len;
    void *(*f_dup)(void*);
    void (*f_free)(void*);
    int (*f_match)(void*, void*);
} SolList;

#define solList_set_head(l, n) (l)->head = n
#define solList_set_tail(l, n) (l)->tail = n
#define solList_set_len(l, c) (l)->len = c
#define solList_incr_len(l) (l)->len++
#define solList_decr_len(l) (l)->len--

#define solList_head(l) (l)->head
#define solList_tail(l) (l)->tail
#define solList_len(l) (l)->len

#define solList_set_val_dup_func(l, f) (l)->f_dup = f
#define solList_set_val_free_func(l, f) (l)->f_free = f
#define solList_set_val_match_func(l, f) (l)->f_match = f

#define solListVal_free_func(l) (l)->f_free
#define solListVal_match_func(l) (l)->f_match

#define solListVal_dup(l, v) (*(l)->f_dup)(v)
#define solListVal_free(l, v) (*(l)->f_free)(v)
#define solListVal_match(l, v1, v2) (*(l)->f_match)(v1, v2)

#define solListNode_set_val(n, v) (n)->val = v
#define solListNode_set_next(n, node) (n)->next = node
#define solListNode_val(n) (n)->val
#define solListNode_next(n) (n)->next

SolList* solList_new();
void solList_free(SolList*);
SolListNode* solList_add(SolList*, void*);
int solList_del_node(SolList*, SolListNode*);
int solList_remove(SolList*, void*);
int solList_has(SolList*, void*);
int solList_attach(SolList*, SolList*);

SolListNode* solListNode_new();
void solListNode_free(SolList*, SolListNode*);

#endif

#ifndef _SOL_LIST_H_
#define _SOL_LIST_H_ 1

#include "sol_common.h"

enum _SolListDir {
    _SolListDirFwd = 1,
    _SolListDirBak = 2,
};

typedef struct _SolListNode {
    void *val;
    struct _SolListNode *pre;
    struct _SolListNode *next;
} SolListNode;

typedef struct _SolList {
    SolListNode *head;
    SolListNode *tail;
    unsigned long len;
    void *(*f_dup)(void*);
    void (*f_free)(void*);
    int (*f_match)(void*);
    void *(*f_mnu)(void*); // match and update
} SolList;

typedef struct _SolListIter {
    SolList *l;
    SolListNode *next;
    enum _SolListDir dir;
} SolListIter;

SolList* solList_new();
void solList_free(SolList*);

#define solList_head(l) l->head
#define solList_tail(l) l->tail
#define solList_len(l) l->len
#define solList_set_free_func(l, f) l->f_free = f
#define solList_set_dup_func(l, f) l->f_dup = f
#define solList_set_match_func(l, f) l->f_match = f
#define solList_set_match_and_up_func(l, f) l->f_mnu = f

#define solListNode_val(n) (n)->val
#define solListNode_next(n) (n)->next
#define solListNodeVal_free(l, n) if (l->f_free) {(*l->f_free)(n->val);}

SolListNode* solList_add(SolList*, void*, enum _SolListDir);
void solList_del_node(SolList*, SolListNode*);
int solList_merge(SolList*, SolList*);

#define solList_add_fwd(l, v) solList_add(l, v, _SolListDirFwd)
#define solList_add_bak(l, v) solList_add(l, v, _SolListDirBak)

SolListIter* solListIter_new(SolList*, enum _SolListDir);
void solListIter_free(SolListIter*);
SolListNode* solListIter_next(SolListIter*);
void solListIter_rewind(SolListIter*);

#endif

#ifndef _SOL_DL_LIST_H_
#define _SOL_DL_LIST_H_ 1

#include "sol_common.h"

enum _SolDlListDir {
    _SolDlListDirFwd = 1,
    _SolDlListDirBak = 2,
};

typedef struct _SolDlListNode {
    void *val;
    struct _SolDlListNode *pre;
    struct _SolDlListNode *next;
} SolDlListNode;

typedef struct _SolDlList {
    SolDlListNode *head;
    SolDlListNode *tail;
    unsigned long len;
    void *(*f_dup)(void*);
    void (*f_free)(void*);
    int (*f_match)(void*);
    void *(*f_mnu)(void*); // match and update
} SolDlList;

typedef struct _SolDlListIter {
    SolDlList *l;
    SolDlListNode *next;
    enum _SolDlListDir dir;
} SolDlListIter;

SolDlList* solDlList_new();
void solDlList_free(SolDlList*);

#define solDlList_head(l) l->head
#define solDlList_tail(l) l->tail
#define solDlList_len(l) l->len
#define solDlList_set_free_func(l, f) l->f_free = f
#define solDlList_set_dup_func(l, f) l->f_dup = f
#define solDlList_set_match_func(l, f) l->f_match = f
#define solDlList_set_match_and_up_func(l, f) l->f_mnu = f

#define solDlListNode_val(n) (n)->val
#define solDlListNode_next(n) (n)->next
#define solDlListNodeVal_free(l, n) if (l->f_free) {(*l->f_free)(n->val);}

SolDlListNode* solDlList_add(SolDlList*, void*, enum _SolDlListDir);
void solDlList_del_node(SolDlList*, SolDlListNode*);
int solDlList_merge(SolDlList*, SolDlList*);

#define solDlList_add_fwd(l, v) solDlList_add(l, v, _SolDlListDirFwd)
#define solDlList_add_bak(l, v) solDlList_add(l, v, _SolDlListDirBak)

SolDlListIter* solDlListIter_new(SolDlList*, enum _SolDlListDir);
void solDlListIter_free(SolDlListIter*);
SolDlListNode* solDlListIter_next(SolDlListIter*);
void solDlListIter_rewind(SolDlListIter*);

#endif

#ifndef _SOL_LIST_H_
#define _SOL_LIST_H_ 1

#include "sol_common.h"

enum SolListDir {
	SolListDirFwd = 1,
	SolListDirBak = 2,
};

typedef struct _SolListNode {
	void *val;
	struct SolListNode *pre;
	struct SolListNode *next;
} SolListNode;

typedef struct _SolList {
	SolListNode *head;
	SolListNode *tail;
	unsigned long len;
	void *(*f_dup)(void *ptr);
	void (*f_free)(void *ptr);
	int (*f_match)(void *ptr);
	void *(*f_mnu)(void *ptr); // match and update
} SolList;

typedef struct _SolListIter {
	struct SolListNode *next;
	enum SolListDirection dir;
} SolListIter;

SolList* solList_new(SolList*);
void solList_free(SolList*);

#define solList_len(l) l->len
#define solList_set_free_func(l, f) l->free = f

SolList* solList_add(SolList*, void*, enum SolListDirection);
void solList_del_node(SolList*, SolListNode*);

SolListIter* solListIter_new(SolList*, enum SolListDirection);
void solListIter_free(SolListIter*);
SolListNode* lolListIter_next(sOLListIter*);
void solListIter_rewind(SolList*, SolListIter*);

#endif

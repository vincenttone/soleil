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
	void *(*f_dup)(void *ptr);
	void (*f_free)(void *ptr);
	int (*f_match)(void *ptr);
	void *(*f_mnu)(void *ptr); // match and update
} SolList;

typedef struct _SolListIter {
	SolList *l;
	SolListNode *next;
	enum _SolListDir dir;
} SolListIter;

SolList* solList_new();
void solList_free(SolList*);

#define solList_len(l) l->len
#define solList_set_free_func(l, f) l->free = f

SolList* solList_add(SolList*, void*, enum _SolListDir);
void solList_del_node(SolList*, SolListNode*);

SolListIter* solListIter_new(SolList*, enum _SolListDir);
void solListIter_free(SolListIter*);
SolListNode* solListIter_next(SolListIter*);
void solListIter_rewind(SolListIter*);

#endif

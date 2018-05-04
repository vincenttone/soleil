#ifndef _SOL_STACK_H_
#define _SOL_STACK_H_ 1

#include <stddef.h>
#include "sol_common.h"
#include "sol_dl_list.h"

typedef struct _SolStackNode {
	void *val;
	struct _SolStackNode *next;
} SolStackNode;

typedef struct _SolStack {
	size_t s;
	SolStackNode *top;
} SolStack;

SolStack* solStack_new();
void solStack_free(SolStack*);
void* solStack_pop(SolStack*);
int solStack_push(SolStack*, void*);
void* solStack_top_val(SolStack*);

SolStackNode* solStackNode_new(void *v, SolStackNode *next);
void solStackNode_free(SolStackNode *n);

#define solStack_is_empty(stk) (stk->s == 0)
#define solStack_size(stk) (stk->s)

#endif

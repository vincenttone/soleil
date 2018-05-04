#include "sol_stack.h"

SolStack* solStack_new()
{
	SolStack *s = sol_calloc(1, sizeof(SolStack));
	if (s == NULL) {
		return NULL;
	}
	s->s = 0;
	return s;
}

void solStack_free(SolStack *s)
{
	SolStackNode *n;
	while ((n = solStack_pop(s))) {
		solStackNode_free(n);
	}
	sol_free(s);
}

void* solStack_pop(SolStack *s)
{
    if (s == NULL || s->s == 0) {
        return NULL;
    }
	SolStackNode *n = s->top;
	s->top = s->top->next;
	s->s--;
    return n->val;
}

int solStack_push(SolStack *s, void *v)
{
    if (s == NULL) {
        return -1;
    }
	if (s->top == NULL) {
		s->top = solStackNode_new(v, NULL);
	} else {
		s->top = solStackNode_new(v, s->top);
	}
	if (s->top == NULL) {
		return 1;
	}
	s->s++;
    return 0;
}

void* solStack_top_val(SolStack *s)
{
	if (s == NULL || s->top == NULL) {
        return NULL;
    }
	return s->top->val;
}

SolStackNode* solStackNode_new(void *v, SolStackNode *next)
{
	SolStackNode *n = sol_alloc(sizeof(SolStackNode));
	if (n == NULL) {
		return NULL;
	}
	n->val = v;
	n->next = next;
	return n;
}

void solStackNode_free(SolStackNode *n)
{
	if (n) {
		sol_free(n);
	}
}

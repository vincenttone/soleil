#include "sol_stack.h"

SolStack* solStack_new()
{
	SolStack *s = sol_alloc(sizeof(SolStack));
	return vListInit(s);
}

void* solStack_pop(SolStack *s)
{
	if (s->tail == NULL) {
		return NULL;
	}
	void *val = s->tail->val;
	vListDelNode(s, s->tail);
	return val;
}

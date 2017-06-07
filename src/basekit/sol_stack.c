#include "sol_stack.h"

SolStack* solStack_new()
{
    SolStack *s = sol_alloc(sizeof(SolStack));
    return solDlList_new(s);
}

void* solStack_pop(SolStack *s)
{
    if (s->tail == NULL) {
        return NULL;
    }
    void *val = s->tail->val;
    solDlList_del_node(s, s->tail);
    return val;
}

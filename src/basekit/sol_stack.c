#include "sol_stack.h"

void* solStack_pop(SolStack *s)
{
    if (s->tail == NULL) {
        return NULL;
    }
    void *val = s->tail->val;
    solDlList_del_node(s, s->tail);
    return val;
}

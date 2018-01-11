#include "sol_stack.h"

void* solStack_pop(SolStack *s)
{
    if (s == NULL || s->tail == NULL) {
        return NULL;
    }
    void *val = s->tail->val;
    solDlList_del_node(s, s->tail);
    return val;
}

int solStack_push(SolStack *s, void *v)
{
    if (s == NULL) {
        return -1;
    }
    if (solDlList_add(s, v, _SolDlListDirFwd)) {
        return 0;
    }
    return 1;
}

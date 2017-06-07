#include <stdio.h>
#include "sol_list.h"

int main()
{
    char *x = "a";
    char *y = "b";
    SolDlList *l = solDlList_new();
    solDlList_add(l, x, _SolDlListDirFwd);
    solDlList_add(l, y, _SolDlListDirFwd);
    solDlList_add(l, x, _SolDlListDirFwd);
    SolDlListIter *i = solDlListIter_new(l, _SolDlListDirFwd);
    solDlListIter_rewind(i);
    SolDlListNode *c;
    while ((c = solDlListIter_next(i))) {
        printf("value is: %s\n", (char*)c->val);
    }
    solDlListIter_free(i);
    solDlList_free(l);
    return 0;
}

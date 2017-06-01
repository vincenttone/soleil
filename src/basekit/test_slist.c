#include <stdio.h>
#include "sol_slist.h"

int main()
{
    char *a = "a";
    char *b = "b";
    char *c = "c";
    SolSlist *l = solSlist_new();
    SolSlistNode *n = solSlist_add(l, a);
    printf("Added node val is [%s]\n", (char*)(solSlistNode_val(n)));
    solSlist_add(l, b);
    solSlist_add(l, c);
    n = solSlist_head(l);
    while (n) {
        printf("The node val is [%s]\n", (char*)(solSlistNode_val(n)));
        n = solSlistNode_next(n);
    }
    solSlist_remove(l, b);
    n = solSlist_head(l);
    while (n) {
        printf("After del node val is [%s]\n", (char*)(solSlistNode_val(n)));
        n = solSlistNode_next(n);
    }
    solSlist_add(l, c);
    solSlist_add(l, b);
    solSlist_add(l, a);
    n = solSlist_head(l);
    while (n) {
        printf("Current node val is [%s]\n", (char*)(solSlistNode_val(n)));
        n = solSlistNode_next(n);
    }
    n = solSlist_head(l);
    solSlist_del_node(l, n);
    n = solSlist_head(l);
    while (n) {
        printf("After del node, val is [%s]\n", (char*)(solSlistNode_val(n)));
        n = solSlistNode_next(n);
    }
    solSlist_free(l);
    return 1;
}

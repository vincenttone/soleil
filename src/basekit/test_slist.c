#include <stdio.h>
#include "sol_slist.h"

#define DC(x) char *x = #x

int main()
{
    DC(a);
    DC(b);
    DC(c);
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
    SolSlist *l1 = solSlist_new();
    DC(d);
    DC(e);
    DC(f);
    solSlist_add(l1, d);
    solSlist_add(l1, e);
    solSlist_add(l1, f);
    n = solSlist_head(l1);
    while (n) {
        printf("L1 node, val is [%s]\n", (char*)(solSlistNode_val(n)));
        n = solSlistNode_next(n);
    }
    int lm = solSlist_merge(l, l1);
    printf("MERGED result %d\n", lm);
    if (lm == 0) {
        n = solSlist_head(l);
        while (n) {
            printf("MERGED l node, val is [%s]\n", (char*)(solSlistNode_val(n)));
            n = solSlistNode_next(n);
        }
    } else {
        solSlist_free(l1);
    }
    solSlist_free(l);
    return 1;
}

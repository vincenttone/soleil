#include <stdio.h>
#include "sol_list.h"

#define DC(x) char *x = #x

int main()
{
    DC(a);
    DC(b);
    DC(c);
    SolList *l = solList_new();
    SolListNode *n = solList_add(l, a);
    printf("Added node val is [%s]\n", (char*)(solListNode_val(n)));
    solList_add(l, b);
    solList_add(l, c);
    n = solList_head(l);
    while (n) {
        printf("The node val is [%s]\n", (char*)(solListNode_val(n)));
        n = solListNode_next(n);
    }
    solList_remove(l, b);
    n = solList_head(l);
    while (n) {
        printf("After del node val is [%s]\n", (char*)(solListNode_val(n)));
        n = solListNode_next(n);
    }
    solList_add(l, c);
    solList_add(l, b);
    solList_add(l, a);
    n = solList_head(l);
    while (n) {
        printf("Current node val is [%s]\n", (char*)(solListNode_val(n)));
        n = solListNode_next(n);
    }
    n = solList_head(l);
    solList_del_node(l, n);
    n = solList_head(l);
    while (n) {
        printf("After del node, val is [%s]\n", (char*)(solListNode_val(n)));
        n = solListNode_next(n);
    }
    SolList *l1 = solList_new();
    DC(d);
    DC(e);
    DC(f);
    solList_add(l1, d);
    solList_add(l1, e);
    solList_add(l1, f);
    n = solList_head(l1);
    while (n) {
        printf("L1 node, val is [%s]\n", (char*)(solListNode_val(n)));
        n = solListNode_next(n);
    }
    int lm = solList_attach(l, l1);
    printf("MERGED result %d\n", lm);
    if (lm == 0) {
        n = solList_head(l);
        while (n) {
            printf("MERGED l node, val is [%s]\n", (char*)(solListNode_val(n)));
            n = solListNode_next(n);
        }
    } else {
        solList_free(l1);
    }
    solList_free(l);
    return 1;
}

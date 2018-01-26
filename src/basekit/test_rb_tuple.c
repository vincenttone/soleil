#include <stdio.h>
#include "sol_rb_tuple.h"

int cmp(void *t1, void *t2, SolRBTuple *t, int flag)
{
	if (*(char*)t1 > *(char*)t2) {
		return 1;
	} else if (*(char*)t1 < *(char*)t2) {
		return -1;
	}
	return 0;
}

void pf(void *v)
{
    if (v)
        printf("try free %s\n", (char*)v);
}

void prv(SolRBTupleRecord *t)
{
    if (t->v) {
        printf("%s\n", (char*)(t->v));
    }
}

int prvx(SolRBTree *tr, SolRBTreeNode *n, void *d)
{
    int *j = (int*)d + 1;
    int *x;
    SolRBTupleRecord *r;
    r = (SolRBTupleRecord*)solRBTreeNode_val(n);
    if (r) {
        for (x = 0; x < j; x++) {
            printf("-");
        }
        printf("%s\n", (char*)(r->v));
    }
    SolRBTree *tree = r->n;
    if (r->n) {
        solRBTree_travelsal_inorder(tree, solRBTree_root(tree), &prvx, j);
    } else {
        //printf("no children\n");
    }
    return 0;
}

void print_rb_tuple(SolRBTuple *t)
{
    int *i = 0;
    SolRBTree *tree = t->n;
    solRBTree_travelsal_inorder(tree, solRBTree_root(tree), &prvx, i);
}

int main()
{
	char *a = "a";
	char *b = "b";
	char *c = "c";
	char *d = "d";
	SolRBTuple *t = solRBTuple_new();
	solRBTuple_set_compare_val_func(t, &cmp);
    solRBTuple_set_free_val_func(t, &pf);
	printf("put ret: %d\n", solRBTuple_put(t, 4, a, b, c, d));
	printf("put ret: %d\n", solRBTuple_put(t, 3, b, c, d));
	solRBTuple_put(t, 3, a, c, d);
	solRBTuple_put(t, 3, b, c, a);
	solRBTuple_put(t, 3, b, d, d);
	solRBTuple_put(t, 4, b, d, a, c);
	solRBTuple_put(t, 3, c, d, a);
    print_rb_tuple(t);

    SolRBTupleRecord *x = solRBTuple_get(t, 2, a, b);
    printf("try get a, b:\n");
    if (x) {
        prv(x);
        prvx(x->n, solRBTree_root(x->n), NULL);
    }
    x = solRBTuple_get(t, 2, a, c);
    printf("Tree is:\n");
    printf("try get a, c:\n");
    if (x) {
        prv(x);
        prvx(x->n, solRBTree_root(x->n), NULL);
    }
    print_rb_tuple(t);
    printf("Remove a,c:\n");
    solRBTuple_remove(t, 2, a, c);
    print_rb_tuple(t);
    x = solRBTuple_get(t, 1, b);
    printf("try get b:\n");
    if (x) {
        prv(x);
        prvx(x->n, solRBTree_root(x->n), NULL);
    }
    printf("Remove b:\n");
    solRBTuple_remove(t, 1, b);
    print_rb_tuple(t);
	solRBTuple_free(t);
	return 0;
}

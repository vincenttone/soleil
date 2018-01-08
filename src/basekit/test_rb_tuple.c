#include <stdio.h>
#include "sol_rb_tuple.h"

int cmp(void *t1, void *t2)
{
    char *c1 = ((SolRBTupleRecord*)t1)->v;
    char *c2 = ((SolRBTupleRecord*)t2)->v;
	if (*c1 > *c2) {
		return 1;
	} else if (*c1 < *c2) {
		return -1;
	}
	return 0;
}

void pf(void *v)
{
    if (v)
        printf("%s\n", (char*)v);
}

void prv(SolRBTupleRecord *t)
{
    pf(t->v);
}

int prvx(SolRBTree *tr, SolRBTreeNode *n, void *d)
{
    int *i = (int*)d + 1;
    SolRBTupleRecord *r = (SolRBTupleRecord*)solRBTreeNode_val(n);
    if (r) {
        while (i--) {
            printf("-");
        }
        printf("%s\n", (char*)(r->v));
    }
    SolRBTree *tree = r->n;
    if (r->n) {
        solRBTree_travelsal_inorder(tree, solRBTree_root(tree), &prvx, i);
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
    // solRBTuple_set_free_val_func(t, &pf);
	printf("put ret: %d\n", solRBTuple_put(t, 4, a, b, c, d));
	printf("put ret: %d\n", solRBTuple_put(t, 3, a, b, d));
    print_rb_tuple(t);
    /*
	solRBTuple_put(t, 3, a, c, d);
	solRBTuple_put(t, 3, b, c, d);
	solRBTuple_put(t, 3, b, d, d);
	solRBTuple_put(t, 4, b, d, a, c);
	solRBTuple_put(t, 3, c, d, a);

    SolRBTupleRecord *x = solRBTuple_get(t, 2, a, b);
    if (x) prv(x);
    */
	solRBTuple_free(t);
	return 0;
}

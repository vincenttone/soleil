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

void pr(SolRBTupleRecord *t)
{
    pf(t->v);
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
    /*
	solRBTuple_put(t, 3, a, c, d);
	solRBTuple_put(t, 3, b, c, d);
	solRBTuple_put(t, 3, b, d, d);
	solRBTuple_put(t, 4, b, d, a, c);
	solRBTuple_put(t, 3, c, d, a);
    SolRBTuple *x = solRBTuple_get(t, 1, a);
    if (x) pr(x);
    */
	solRBTuple_free(t);
	return 0;
}

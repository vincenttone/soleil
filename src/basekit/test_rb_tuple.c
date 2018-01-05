#include <stdio.h>
#include "sol_rb_tuple.h"

int cmp(void *a, void *b)
{
	if (*(char*)a > *(char*)b) {
		return 1;
	} else if (*(char*)a < *(char*)b) {
		return -1;
	}
	return 0;
}

int main()
{
	char *a = "a";
	char *b = "b";
	char *c = "c";
	char *d = "d";
	SolRBTuple *t = solRBTuple_new();
	solRBTuple_set_compare_val_func(t, &cmp);
	solRBTuple_put(t, 3, a, b, c);
	solRBTuple_put(t, 3, a, b, d);
	solRBTuple_put(t, 3, a, c, d);
	solRBTuple_put(t, 3, b, c, d);
	solRBTuple_put(t, 3, b, d, d);
	solRBTuple_put(t, 4, b, d, a, c);
	solRBTuple_put(t, 3, c, d, a);
	solRBTuple_free(t);
	return 0;
}

#include <stdio.h>
#include "sol_list.h"

int main()
{
	char *x = "a";
	char *y = "b";
	SolList *l = solList_new();
	solList_add(l, x, _SolListDirFwd);
	solList_add(l, y, _SolListDirFwd);
	solList_add(l, x, _SolListDirFwd);
	SolListIter *i = solListIter_new(l, _SolListDirFwd);
	solListIter_rewind(i);
	SolListNode *c;
	while ((c = solListIter_next(i))) {
		printf("value is: %s\n", (char*)c->val);
	}
	solListIter_free(i);
	solList_free(l);
	return 0;
}

#include <stdio.h>
#include "sol_stack.h"

int main()
{
	SolStack *s = solStack_new();
	printf("pop value: %s\n", (char*)solStack_pop(s));
	solStack_push(s, "data1");
	printf("pop value: %s\n", (char*)solStack_pop(s));
	printf("pop value: %s\n", (char*)solStack_pop(s));
	solStack_push(s, "data2");
	printf("pop value: %s\n", (char*)solStack_pop(s));
	printf("pop value: %s\n", (char*)solStack_pop(s));
	printf("pop value: %s\n", (char*)solStack_pop(s));
	solStack_push(s, "data1");
	solStack_push(s, "data2");
	solStack_push(s, "data3");
	solStack_push(s, "data4");
	solStack_push(s, "data5");
	solStack_push(s, "data6");
	printf("pop value: %s\n", (char*)solStack_pop(s));
	printf("pop value: %s\n", (char*)solStack_pop(s));
	solStack_push(s, "data7");
	solStack_push(s, "data8");
	solStack_push(s, "data9");
	solStack_push(s, "data10");
	void *d;
	while ((d = solStack_pop(s))) {
		printf("pop value: %s\n", (char*)d);
	}
	printf("pop value: %s\n", (char*)solStack_pop(s));
	printf("pop value: %s\n", (char*)solStack_pop(s));
	printf("pop value: %s\n", (char*)solStack_pop(s));
	solStack_free(s);
	return 0;
}

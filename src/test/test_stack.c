#include <stdio.h>
#include "sol_stack.h"

int main()
{
    SolStack *s = solStack_new();
    printf("pop value: %s\n", (char*)solStack_pop(s));
	printf("push data1\n");
    solStack_push(s, "data1");
    printf("pop value: %s\n", (char*)solStack_pop(s));
    printf("pop value: %s\n", (char*)solStack_pop(s));
	printf("push data2\n");
    solStack_push(s, "data2");
    printf("pop value: %s\n", (char*)solStack_pop(s));
    printf("pop value: %s\n", (char*)solStack_pop(s));
    printf("pop value: %s\n", (char*)solStack_pop(s));
	printf("push data1\n");
    solStack_push(s, "data1");
	printf("push data2\n");
    solStack_push(s, "data2");
	printf("push data3\n");
    solStack_push(s, "data3");
	printf("push data4\n");
    solStack_push(s, "data4");
	printf("push data5\n");
    solStack_push(s, "data5");
	printf("push data6\n");
    solStack_push(s, "data6");
    printf("pop value: %s\n", (char*)solStack_pop(s));
    printf("pop value: %s\n", (char*)solStack_pop(s));
	printf("push data7\n");
    solStack_push(s, "data7");
	printf("push data8\n");
    solStack_push(s, "data8");
	printf("push data9\n");
    solStack_push(s, "data9");
	printf("push data10\n");
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

#include <stdio.h>
#include "sol_table_fixed.h"

#define RS 8
#define CS 4
#define AS 32
int main()
{
	char a[CS][RS] = {};
	size_t x, y;
	char al = 'A';
	for (x = 0; x < CS; x++) {
		for (y = 0; y < RS; y++) {
			a[x][y] = al++;
			printf("%c", a[x][y]);
		}
		printf("\n");
	}
	printf("\n");
	SolTableFixed *t = solTableFixed_new(CS, RS);
	for (x = 0; x < CS; x++) {
		for (y = 0; y < RS; y++) {
			// printf("put %c to %zu:%zu\n", (a[x][y]), x, y);
			solTableFixed_put(t, x, y, &(a[x][y]));
		}
	}
	printf("table size columns %zu, rows %zu\n", t->cs, t->rs);
	char *c;
	printf("datas in mem: ");
	for (x = 0; x < t->cs * t->rs; x++) {
		printf("[%c] ", **(char**)(t->vals + x));
	}
	printf("\n");
	for (x = 0; x < CS; x++) {
		for (y = 0; y < RS; y++) {
			c = solTableFixed_get(t, x, y);
			printf("[%c] \t", *c);
		}
		printf("\n");
	}
	SolTableFixedIter *i;
	for (x = 0; x < RS; x++) {
		printf("=======get columns of row: %zu =========\n", x);
		i = solTableFixed_get_columns(t, x);
		do {
			c = solTableFixedIter_current(i);
			if (c == NULL) {
				printf("get char: NULL\n");
			} else {
				printf("get char: %zu --- -> %c\n", x, *c);
			}
		} while (solTableFixedIter_next(i) == 0);
		solTableFixedIter_free(i);
	}
	solTableFixed_free(t);
	return 0;
}

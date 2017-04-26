#include <stdio.h>
#include <string.h>
#include "sol_pattern.h"

int main()
{
	int r;
	char *t1 = "abcabc";
	size_t tl1 = strlen(t1);
	char *t2 = "a";
	size_t tl2 = strlen(t2);
	char *a = "a";
	SolPatternStateGen *g = solPatternStateGen_new();
	SolPattern *p1 = solPattern_empty_new(g);
	SolPattern *p2 = solPattern_literal_new(g, a);
	r = solPattern_match(p1, NULL, 0);
	printf("//\t''\t%d\n", r);
	r = solPattern_match(p1, t1, tl1);
	printf("//\t%s?\t%d\n", t1, r);
	r = solPattern_match(p2, t1, tl1);
	printf("/%s/\t[%s]?\t%d\n", a, t1, r);
	r = solPattern_match(p2, t2, tl2);
	printf("/%s/\t[%s]?\t%d\n", a, t2, r);
	solPatternStateGen_free(g);
	solPattern_free(p1);
	solPattern_free(p2);
	return 0;
}

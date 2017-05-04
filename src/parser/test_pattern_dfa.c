#include <string.h>
#include <stdio.h>
#include "sol_pattern_dfa.h"

int main()
{
	char *s0 = "";
	char *sa = "a";
	char *sab = "ab";
	char *saaa = "aaa";
	size_t l0 = strlen(s0);
	size_t l1 = strlen(sa);
	size_t l2 = strlen(sab);
	size_t l3 = strlen(saaa);
	SolPatternStateGen *g = solPatternStateGen_new();
	SolPattern *empty = solPattern_empty_new(g);
	printf("//\t\"%s\"\tmatch? %d\n", s0, solPattern_is_match(empty, s0, l0));
	printf("//\t\"%s\"\tmatch? %d\n", sa, solPattern_is_match(empty, sa, l1));
	printf("//\t\"%s\"\tmatch? %d\n", sab, solPattern_is_match(empty, sab, l2));
	solPattern_free(empty);
	SolPattern *pa = solPattern_literal_new(g, sa);
	printf("/a/\t\"%s\"\tmatch? %d\n", s0, solPattern_is_match(pa, 	s0, l0));
	printf("/a/\t\"%s\"\tmatch? %d\n", sa, solPattern_is_match(pa, 	sa, l1));
	printf("/a/\t\"%s\"\tmatch? %d\n", sab, solPattern_is_match(pa, sab, l2));
	solPattern_free(pa);
	SolPattern *prpt = solPattern_repeat(solPattern_literal_new(g, sa));
	_solPattern_debug_relations(prpt);
	printf("/a*/\t\"%s\"\tmatch? %d\n", s0, solPattern_is_match(pa, 	s0, l0));
	printf("/a*/\t\"%s\"\tmatch? %d\n", sa, solPattern_is_match(pa, 	sa, l1));
	printf("/a*/\t\"%s\"\tmatch? %d\n", sab, solPattern_is_match(pa, 	sab, l2));
	printf("/a*/\t\"%s\"\tmatch? %d\n", saaa, solPattern_is_match(pa, 	saaa, l3));
	solPattern_free(prpt);
	solPatternStateGen_free(g);
	return 0;
}

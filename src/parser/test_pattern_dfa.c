#include <string.h>
#include <stdio.h>
#include "sol_pattern_dfa.h"

int main()
{
	char *s0 = "";
	char *sa = "a";
	char *sb = "b";
	char *sc = "c";
	char *sab = "ab";
	char *saaa = "aaa";
	char *sabc = "abc";
	char *sabcabcabc = "abcabcabc";
	char *sabacabac = "abacabac";
	size_t l0 = strlen(s0);
	size_t l1 = strlen(sa);
	size_t l2 = strlen(sab);
	size_t l3 = strlen(saaa);
	size_t l8 = strlen(sabacabac);
	size_t l9 = strlen(sabcabcabc);
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
	printf("/a*/\t\"%s\"\tmatch? %d\n", s0, solPattern_is_match(prpt, 	s0, l0));
	printf("/a*/\t\"%s\"\tmatch? %d\n", sa, solPattern_is_match(prpt, 	sa, l1));
	printf("/a*/\t\"%s\"\tmatch? %d\n", sab, solPattern_is_match(prpt, 	sab, l2));
	printf("/a*/\t\"%s\"\tmatch? %d\n", saaa, solPattern_is_match(prpt, 	saaa, l3));
	solPattern_free(prpt);
	SolPattern *pab = solPattern_concatenate(solPattern_literal_new(g, sa), solPattern_literal_new(g, sb));
	printf("/ab/\t\"%s\"\tmatch? %d\n", s0, solPattern_is_match(pab, 	s0, l0));
	printf("/ab/\t\"%s\"\tmatch? %d\n", sa, solPattern_is_match(pab, 	sa, l1));
	printf("/ab/\t\"%s\"\tmatch? %d\n", sab, solPattern_is_match(pab, 	sab, l2));
	printf("/ab/\t\"%s\"\tmatch? %d\n", saaa, solPattern_is_match(pab, 	saaa, l3));
	printf("/ab/\t\"%s\"\tmatch? %d\n", sabc, solPattern_is_match(pab, 	sabc, l3));
	SolPattern *pabc = solPattern_concatenate(pab, solPattern_literal_new(g, sc));
	printf("/abc/\t\"%s\"\tmatch? %d\n", s0, solPattern_is_match(pab, 	s0, l0));
	printf("/abc/\t\"%s\"\tmatch? %d\n", sa, solPattern_is_match(pab, 	sa, l1));
	printf("/abc/\t\"%s\"\tmatch? %d\n", sab, solPattern_is_match(pab, 	sab, l2));
	printf("/abc/\t\"%s\"\tmatch? %d\n", saaa, solPattern_is_match(pab, 	saaa, l3));
	printf("/abc/\t\"%s\"\tmatch? %d\n", sabc, solPattern_is_match(pab, 	sabc, l3));
	SolPattern *prptabc = solPattern_repeat(pabc);
	printf("/abc*/\t\"%s\"\tmatch? %d\n", s0, solPattern_is_match(pab, 	s0, l0));
	printf("/abc*/\t\"%s\"\tmatch? %d\n", sa, solPattern_is_match(pab, 	sa, l1));
	printf("/abc*/\t\"%s\"\tmatch? %d\n", sab, solPattern_is_match(pab, 	sab, l2));
	printf("/abc*/\t\"%s\"\tmatch? %d\n", saaa, solPattern_is_match(pab, 	saaa, l3));
	printf("/abc*/\t\"%s\"\tmatch? %d\n", sabc, solPattern_is_match(pab, 	sabc, l3));
	printf("/abc*/\t\"%s\"\tmatch? %d\n", sabacabac, solPattern_is_match(pab, 	sabacabac, l8));
	printf("/abc*/\t\"%s\"\tmatch? %d\n", sabcabcabc, solPattern_is_match(pab, 	sabcabcabc, l9));
	solPattern_free(prptabc);
	pa = solPattern_literal_new(g, sa);
	SolPattern *pb = solPattern_literal_new(g, sb);
	SolPattern *pa_b = solPattern_choose(pa, pb);
	printf("/a|b/\t\"%s\"\tmatch? %d\n", s0, solPattern_is_match(pa_b, 	s0, l0));
	printf("/a|b/\t\"%s\"\tmatch? %d\n", sa, solPattern_is_match(pa_b, 	sa, l1));
	printf("/a|b/\t\"%s\"\tmatch? %d\n", sab, solPattern_is_match(pa_b, 	sab, l2));
	printf("/a|b/\t\"%s\"\tmatch? %d\n", saaa, solPattern_is_match(pa_b, 	saaa, l3));
	solPattern_free(pa_b);
	solPatternStateGen_free(g);
	return 0;
}

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
	char *t3 = "ab";
	size_t tl3 = strlen(t3);
	char *s0 = "";
	char *a = "a";
	char *b = "b";
	char *c = "c";
	char *ab = "ab";
	char *abc = "abc";
	char *aaa = "aaa";
	char *ac = "ac";
	size_t l0 = strlen(s0);
	size_t l1 = strlen(a);
	size_t l2 = strlen(ab);
	size_t l3 = strlen(aaa);
	SolPatternStateGen *g = solPatternStateGen_new();
	SolPattern *p1 = solPattern_empty_new(g);
	SolPattern *p2 = solPattern_literal_new(g, a);
	SolPattern *p3 = solPattern_literal_new(g, b);
	r = solPattern_match(p1, s0, l0);
	printf("//\t''\t%d\n", r);
	r = solPattern_match(p1, t1, tl1);
	printf("//\t%s?\t%d\n", t1, r);
	r = solPattern_match(p2, t1, tl1);
	printf("/%s/\t[%s]?\t%d\n", a, t1, r);
	r = solPattern_match(p2, t2, tl2);
	printf("/%s/\t[%s]?\t%d\n", a, t2, r);
	solPattern_concatenate(p2, p3);
	r = solPattern_match(p2, t1, tl1);
	printf("/ab/\t[%s]?\t%d\n", t1, r);
	r = solPattern_match(p2, t2, tl2);
	printf("/ab/\t[%s]?\t%d\n", t2, r);
	r = solPattern_match(p2, t3, tl3);
	printf("/ab/\t[%s]?\t%d\n", t3, r);
	solPattern_free(p2);
	SolPattern *p4 = solPattern_literal_new(g, a);
	solPattern_choose(p4, solPattern_literal_new(g, b));
	printf("/a|b/\t[%s]?\t%d\n", a, solPattern_match(p4, t2, tl2));
	printf("/a|b/\t[%s]?\t%d\n", b, solPattern_match(p4, b, tl2));
	printf("/a|b/\t[%s]?\t%d\n", t1, solPattern_match(p4, t1, tl1));
	solPattern_free(p4);
	SolPattern *p5 = solPattern_literal_new(g, a);
	solPattern_repeat(p5);
	printf("/a+/\t[%s]?\t%d\n", a, solPattern_match(p5, t2, tl2));	
	printf("/a+/\t[%s]?\t%d\n", b, solPattern_match(p5, b, tl2));
	printf("/a+/\t[%s]?\t%d\n", aaa, solPattern_match(p5, aaa, l3));
	solPattern_choose(p1, p5);
	printf("/a*/\t[%s]?\t%d\n", aaa, solPattern_match(p1, aaa, l3));
	printf("/a*/\t[%s]?\t%d\n", s0, solPattern_match(p1, s0, l0));
	solPattern_free(p1);
	// solPattern_empty_new(g);
	// solPattern_literal_new(g, a);
	// solPattern_literal_new(g, b);
	// solPattern_literal_new(g, c);
	
	SolPattern *p6;
	p6 = solPattern_choose(solPattern_concatenate(solPattern_concatenate(solPattern_literal_new(g, a),
																		 solPattern_literal_new(g, b)),
												  solPattern_literal_new(g, c)),
						   solPattern_concatenate(solPattern_literal_new(g, a),
												  solPattern_literal_new(g, c)));
	//_solPattern_debug_relations(p6);
	printf("/(abc)|(ac)/\t[%s]?\t%d\n", a, solPattern_match(p6, a, l1));
	printf("/(abc)|(ac)/\t[%s]?\t%d\n", ab, solPattern_match(p6, ab, l2));	
	printf("/(abc)|(ac)/\t[%s]?\t%d\n", ac, solPattern_match(p6, ac, l2));
	printf("/(abc)|(ac)/\t[%s]?\t%d\n", abc, solPattern_match(p6, abc, l3));
	solPattern_repeat(p6);
	//_solPattern_debug_relations(p6);
	printf("/((abc)|(ac))+/\t[%s]?\t%d\n", ac, solPattern_match(p6, ac, l2));
	printf("/((abc)|(ac))+/\t[%s]?\t%d\n", abc, solPattern_match(p6, abc, l3));
	char *abc_x3 = "abcabcabc";
	char *abc_ac = "abcac";
	char *abc_ac_r2 = "abcacacabc";
	size_t l9 = strlen(abc_x3);
	size_t l5 = strlen(abc_ac);
	size_t l10 = strlen(abc_ac_r2);
	printf("/((abc)|(ac))+/\t[%s]?\t%d\n", abc, solPattern_match(p6, abc, l3));
	printf("/((abc)|(ac))+/\t[%s]?\t%d\n", abc_x3, solPattern_match(p6, abc_x3, l9));
	printf("/((abc)|(ac))+/\t[%s]?\t%d\n", abc_ac, solPattern_match(p6, abc_ac, l5));
	printf("/((abc)|(ac))+/\t[%s]?\t%d\n", abc_ac_r2, solPattern_match(p6, abc_ac_r2, l10));
	solPattern_free(p6);
	solPatternStateGen_free(g);
	return 0;
}

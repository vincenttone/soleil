#include <stdio.h>
#include "sol_fa.h"

#define solFaRule_inspect(r) printf("--FA: state <%ld> -> <%ld>, character: <%s>--\n", \
									  r->state, r->next_state, r->character)
int match(SolFaRule*, SOL_FA_STATE, SOL_FA_CHARACTER);
int match(SolFaRule *r, SOL_FA_STATE s, SOL_FA_CHARACTER c)
{
	if ( r->state == s && r->character == c) {
		return 0;
	}
	return 1;
}

int main()
{
	SOL_FA_STATE i = 1;
	SOL_FA_STATE j = 2;
	SOL_FA_CHARACTER s1 = "a";
	SOL_FA_CHARACTER s2 = "b";
	SolFaRule *rule = solFaRule_new(i, i, s2);
	solFaRule_set_match_func(rule, &match);
	solFaRule_inspect(rule);
	printf("Rule applies %s? %d\n", s1, solFaRule_applies_to(rule, i, s1));
	printf("Rule applies %s? %d\n", s2, solFaRule_applies_to(rule, i, s2));
	printf("Rule applies %s? %d\n", s1, solFaRule_applies_to(rule, j, s1));
	printf("Rule applies %s? %d\n", s2, solFaRule_applies_to(rule, j, s2));
	solFaRule_free(rule);
	return 0;
}

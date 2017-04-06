#include <stdio.h>
#include "sol_dfa.h"

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
	SOL_FA_STATE one = 1;
	SOL_FA_STATE two = 2;
	SOL_FA_STATE three = 3;

	SOL_FA_CHARACTER a = "a";
	SOL_FA_CHARACTER b = "b";
	SOL_FA_CHARACTER c = "c";
	
	SolFaRule *rule1 = solFaRule_new(one, one, a);
	SolFaRule *rule2 = solFaRule_new(one, two, b);
	SolFaRule *rule3 = solFaRule_new(two, two, b);
	SolFaRule *rule4 = solFaRule_new(two, three, c);
	SolFaRule *rule5 = solFaRule_new(three, three, c);
	solFaRule_set_match_func(rule1, &match);
	solFaRule_set_match_func(rule2, &match);
	solFaRule_set_match_func(rule3, &match);
	solFaRule_set_match_func(rule4, &match);
	solFaRule_set_match_func(rule5, &match);
	solFaRule_inspect(rule1);
	printf("rule4 applies to 2 & c? <%d>\n", solFaRule_applies_to(rule4, 2, c));
	printf("rule4 applies to 2 & b? <%d>\n", solFaRule_applies_to(rule4, 2, b));
	SolDfaRuleBook *book = solDfaRuleBook_new();
	solDfaRuleBook_add_rule(book, rule1);
	solDfaRuleBook_add_rule(book, rule2);
	solDfaRuleBook_add_rule(book, rule3);
	solDfaRuleBook_add_rule(book, rule4);
	solDfaRuleBook_add_rule(book, rule5);
	printf("next state of 1 & a -> %ld\n", solDfaRuleBook_next_state(book, one, a));
	printf("next state of 1 & b -> %ld\n", solDfaRuleBook_next_state(book, one, b));
	printf("next state of 2 & b -> %ld\n", solDfaRuleBook_next_state(book, two, c));
	SolFaRule *r1 = solDfaRuleBook_rule_for(book, two, c);
	printf("rule for 2 & c is: ----");
	solFaRule_inspect(r1);
	r1 = solDfaRuleBook_rule_for(book, three, c);
	
	printf("rule for 3 & c is: ----");
	solFaRule_inspect(r1);

	SolDfa *dfa = solDfa_new(book, one, three);
	solDfa_read_character(dfa, a);
	printf("DFA input <%s>, state now is: %ld accepting? <%d>\n", a,  dfa->current_state, solDfa_is_accepting(dfa));
	solDfa_read_character(dfa, c);
	printf("DFA input <%s>, state now is: %ld accepting? <%d>\n", c,  dfa->current_state, solDfa_is_accepting(dfa));
	solDfa_read_character(dfa, b);
	printf("DFA input <%s>, state now is: %ld accepting? <%d>\n", b,  dfa->current_state, solDfa_is_accepting(dfa));
	solDfa_read_character(dfa, c);
	printf("DFA input <%s>, state now is: %ld accepting? <%d>\n", c,  dfa->current_state, solDfa_is_accepting(dfa));

	solDfa_free(dfa);
	return 0;
}

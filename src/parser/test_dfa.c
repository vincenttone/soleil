#include <stdio.h>
#include "sol_dfa.h"

int main()
{
	SOL_FA_STATE one = 1;
	SOL_FA_STATE two = 2;
	SOL_FA_STATE three = 3;

	SOL_FA_CHARACTER a = "a";
	SOL_FA_CHARACTER b = "b";
	SOL_FA_CHARACTER c = "c";
	
	SolFaRule *rule1 = sol_new_fa_rule(one, one, a);
	SolFaRule *rule2 = sol_new_fa_rule(one, two, b);
	SolFaRule *rule3 = sol_new_fa_rule(two, two, b);
	SolFaRule *rule4 = sol_new_fa_rule(two, three, c);
	SolFaRule *rule5 = sol_new_fa_rule(three, three, c);
	sol_fa_rule_inspect(rule1);
	printf("rule4 applies to 2 & c? <%d>\n", sol_fa_rule_applies_to(rule4, 2, c));
	printf("rule4 applies to 2 & b? <%d>\n", sol_fa_rule_applies_to(rule4, 2, b));
	SolDfaRuleBook *book = sol_dfa_rule_book_new();
	sol_dfa_rule_book_add_rule(book, rule1);
	sol_dfa_rule_book_add_rule(book, rule2);
	sol_dfa_rule_book_add_rule(book, rule3);
	sol_dfa_rule_book_add_rule(book, rule4);
	sol_dfa_rule_book_add_rule(book, rule5);
	printf("next state of 1 & a -> %ld\n", sol_dfa_rule_book_next_state(book, one, a));
	printf("next state of 1 & b -> %ld\n", sol_dfa_rule_book_next_state(book, one, b));
	printf("next state of 2 & b -> %ld\n", sol_dfa_rule_book_next_state(book, two, c));
	SolFaRule *r1 = sol_dfa_rule_book_rule_for(book, two, c);
	printf("rule for 2 & c is: ----");
	sol_fa_rule_inspect(r1);
	r1 = sol_dfa_rule_book_rule_for(book, three, c);
	
	printf("rule for 3 & c is: ----");
	sol_fa_rule_inspect(r1);

	SolDfa *dfa = sol_dfa_new(book, one, three);
	sol_dfa_read_character(dfa, a);
	printf("DFA input <%s>, state now is: %ld accepting? <%d>\n", a,  dfa->current_state, sol_dfa_is_accepting(dfa));
	sol_dfa_read_character(dfa, c);
	printf("DFA input <%s>, state now is: %ld accepting? <%d>\n", c,  dfa->current_state, sol_dfa_is_accepting(dfa));
	sol_dfa_read_character(dfa, b);
	printf("DFA input <%s>, state now is: %ld accepting? <%d>\n", b,  dfa->current_state, sol_dfa_is_accepting(dfa));
	sol_dfa_read_character(dfa, c);
	printf("DFA input <%s>, state now is: %ld accepting? <%d>\n", c,  dfa->current_state, sol_dfa_is_accepting(dfa));

	sol_dfa_release(dfa);
	return 0;
}

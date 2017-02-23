#include <stdio.h>
#include "sol_dfa.h"

int main()
{
	SolFaRule *rule1 = sol_new_fa_rule(1, 1, 'a');
	SolFaRule *rule2 = sol_new_fa_rule(1, 2, 'b');
	SolFaRule *rule3 = sol_new_fa_rule(2, 2, 'b');
	SolFaRule *rule4 = sol_new_fa_rule(2, 3, 'c');
	SolFaRule *rule5 = sol_new_fa_rule(3, 3, 'c');
	char *i = sol_fa_rule_inspect(rule1);
	printf("%s", i);
	sol_dfa_free_inspect(i);
	printf("rule4 applies to 2 & c? <%d>\n", sol_fa_rule_applies_to(rule4, 2, 'c'));
	printf("rule4 applies to 2 & b? <%d>\n", sol_fa_rule_applies_to(rule4, 2, 'b'));
	SolDfaRuleBook *book = sol_dfa_rule_book_new();
	sol_dfa_rule_book_add_rule(book, rule1);
	sol_dfa_rule_book_add_rule(book, rule2);
	sol_dfa_rule_book_add_rule(book, rule3);
	sol_dfa_rule_book_add_rule(book, rule4);
	sol_dfa_rule_book_add_rule(book, rule5);
	char *j = sol_dfa_rule_book_inspect(book);
	printf("%s", j);
	printf("next state of 1 & a -> %ld\n", sol_dfa_rule_book_next_state(book, 1, 'a'));
	printf("next state of 1 & b -> %ld\n", sol_dfa_rule_book_next_state(book, 1, 'b'));
	printf("next state of 2 & b -> %ld\n", sol_dfa_rule_book_next_state(book, 2, 'c'));
	SolFaRule *r1 = sol_dfa_rule_book_rule_for(book, 2, 'c');
	i = sol_fa_rule_inspect(r1);
	printf("rule for 2 & c is: %s", i);
	r1 = sol_dfa_rule_book_rule_for(book, 3, 'c');
	i = sol_fa_rule_inspect(r1);
	printf("rule for 3 & c is: %s", i);
	sol_dfa_free_inspect(i);
	sol_dfa_rule_book_release(book);
	return 0;
}

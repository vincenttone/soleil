#include <stdio.h>
#include "sol_dfa.h"

int main()
{
	SOL_FA_STATE s1 = 1, s2 = 2;
	SOL_FA_CHARACTER c1 = 'a';
	SolFaRule *rule = sol_new_fa_rule(s1, s2, c1);
	SOL_FA_STATE sr1 = sol_fa_rule_current_state(rule);
	SOL_FA_STATE sr2 = sol_fa_rule_next_state(rule);
	SOL_FA_CHARACTER cr1 = sol_fa_rule_character(rule);
	printf("state <%ld> -> <%ld>, wanna <%c>\n", sr1, sr2, cr1);
	sol_destory_fa_rule(rule);
	return 0;
}

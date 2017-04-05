#include <stdio.h>
#include "sol_fa.h"

int main()
{
	SOL_FA_STATE i = 1;
	SOL_FA_CHARACTER s = "a";
	SolFaRule *rule = sol_new_fa_rule(i, i, s);
	sol_fa_rule_inspect(rule);
	sol_destory_fa_rule(rule);
	return 0;
}

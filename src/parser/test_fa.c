#include <stdio.h>
#include "sol_fa.h"

int main()
{
	SolFaRule *rule = sol_new_fa_rule(1, 1, 'a');
	char *i = sol_fa_rule_inspect(rule);
	printf("%s", i);
	sol_fa_free_inspect(i);
	sol_destory_fa_rule(rule);
	return 0;
}

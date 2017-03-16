#include "sol_fa.h"

SolFaRule* sol_new_fa_rule(SOL_FA_STATE state, SOL_FA_STATE next_state, SOL_FA_CHARACTER character)
{
	SolFaRule *rule;
	rule = sol_alloc(sizeof(SolFaRule));
	if (rule == NULL) {
		return NULL;
	}
	rule->state = state;
	rule->next_state = next_state;
	rule->character = character;
	return rule;
}

void sol_destory_fa_rule(SolFaRule *rule)
{
	sol_free(rule);
}

bool sol_fa_rule_applies_to(SolFaRule *rule, SOL_FA_STATE state, SOL_FA_CHARACTER character)
{
	if (rule->state == state && sol_fa_rule_character_match(rule, character)) {
		return true;
	}
	return false;
}

SOL_FA_STATE sol_fa_rule_current_state(SolFaRule *rule)
{
	return rule->state;
}

SOL_FA_STATE sol_fa_rule_next_state(SolFaRule *rule)
{
	return rule->next_state;
}

SOL_FA_CHARACTER sol_fa_rule_character(SolFaRule *rule)
{
	return rule->character;
}

char* sol_fa_rule_inspect(SolFaRule *rule)
{
	char *i = sol_alloc(sizeof(char) * SOL_FA_RULE_INS_LEN);
	sprintf(i, "--FA: state <%ld> -> <%ld>, character: <%c>--\n",
			rule->state, rule->next_state, rule->character);
	return i;
}

void sol_fa_free_inspect(char *i)
{
	sol_free(i);
}

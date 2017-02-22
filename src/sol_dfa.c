#include <stdio.h>
#include "sol_dfa.h"

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

bool sol_fa_rule_applies_to(SolFaRule *rule, SOL_FA_CHARACTER character, SOL_FA_STATE state)
{
	if (rule->state == state && rule->character == character) {
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

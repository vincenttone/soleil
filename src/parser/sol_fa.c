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
bool sol_fa_rule_match(SolFaRule *r, SOL_FA_CHARACTER c)
{
	if (c == SolNil) { //free moves
		return true;
	}
	if (sol_fa_rule_character_match(r, c)) {
		return true;
	}
	return false;
}

bool sol_fa_rule_applies_to(SolFaRule *rule, SOL_FA_STATE state, SOL_FA_CHARACTER character)
{
	if (rule->state == state && sol_fa_rule_match(rule, character)) {
		return true;
	}
	return false;
}

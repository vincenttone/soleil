#include "sol_fa.h"

SolFaRule* solFaRule_new(SOL_FA_STATE state, SOL_FA_STATE next_state, SOL_FA_CHARACTER character)
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

void solFaRule_free(SolFaRule *rule)
{
	sol_free(rule);
}

bool solFaRule_applies_to(SolFaRule *rule, SOL_FA_STATE state, SOL_FA_CHARACTER character)
{
	if (rule->state == state && rule->match(rule->character, character) == 0) {
		return true;
	}
	return false;
}

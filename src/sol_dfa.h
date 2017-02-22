#ifndef SOL_DFA_H
#define SOL_DFA_H 1

#include <stdlib.h>
#include <stdbool.h>

#include "list.h"

#define sol_alloc malloc
#define sol_free free

#define SOL_FA_CHARACTER char
#define SOL_FA_STATE long

typedef struct SolFaRule {
	SOL_FA_STATE state;
	SOL_FA_STATE next_state;
	SOL_FA_CHARACTER character;
} SolFaRule;

typedef struct SolFaRuleBook {
	vList *rules;
} SolDfaRuleBook;

typedef struct SolDfa {
	SolDfaRuleBook *rule_book;
} SolDfa;

SolFaRule* sol_new_fa_rule(SOL_FA_STATE state, SOL_FA_STATE next_state, SOL_FA_CHARACTER character);
void sol_destory_fa_rule(SolFaRule *rule);
SOL_FA_STATE sol_fa_rule_current_state(SolFaRule *rule);
SOL_FA_STATE sol_fa_rule_next_state(SolFaRule *rule);
SOL_FA_CHARACTER sol_fa_rule_character(SolFaRule *rule);
bool sol_fa_rule_applies_to(SolFaRule *rule, SOL_FA_CHARACTER character, SOL_FA_STATE state);

#endif

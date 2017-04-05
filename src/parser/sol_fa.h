#ifndef _SOL_FA_H_
#define _SOL_FA_H_ 1

#include <stdio.h>
#include <stdbool.h>
#include "sol_common.h"
#include "sol_utils.h"

#define SOL_FA_CHARACTER char*
#define SOL_FA_STATE long

#define SOL_FA_STATE_NONE 0

#define SOL_FA_RULE_INS_LEN 60

typedef struct SolFaRule {
	SOL_FA_STATE state;
	SOL_FA_STATE next_state;
	SOL_FA_CHARACTER character;
} SolFaRule;

SolFaRule* sol_new_fa_rule(SOL_FA_STATE state, SOL_FA_STATE next_state, SOL_FA_CHARACTER character);
void sol_destory_fa_rule(SolFaRule *rule);

#define sol_fa_rule_current_state(r) r->state
#define sol_fa_rule_next_state(r) r->next_state
#define sol_fa_rule_character(r) r->character

bool sol_fa_rule_applies_to(SolFaRule *rule, SOL_FA_STATE state, SOL_FA_CHARACTER character);
bool sol_fa_rule_match(SolFaRule*, SOL_FA_CHARACTER);
#define sol_fa_rule_character_match(r, c) (r->character == c)

#define sol_fa_rule_inspect(r) printf("--FA: state <%ld> -> <%ld>, character: <%s>--\n", \
									  r->state, r->next_state, r->character)

#endif

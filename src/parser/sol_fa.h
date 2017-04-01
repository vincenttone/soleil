#ifndef _SOL_FA_H_
#define _SOL_FA_H_ 1

#include <stdio.h>
#include <stdbool.h>
#include "sol_common.h"
#include "sol_utils.h"

#define SOL_FA_CHARACTER char
#define SOL_FA_STRING char*
#define SOL_FA_STATE unsigned long
#define SOL_FA_CHARACTER_PTR SOL_FA_CHARACTER*
#define SOL_FA_STATE_PTR SOL_FA_STATE*

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
char* sol_fa_rule_inspect(SolFaRule *rule);
void sol_fa_free_inspect(char *i);

#define __sol_debug_print_fa_rule(r) do{		\
		char *__debug_i;						\
		__debug_i = sol_fa_rule_inspect(r);		\
		printf("DEBUG: %s", __debug_i);			\
		sol_fa_free_inspect(__debug_i);			\
}while(0)

#define sol_fa_rule_character_match(r, c) (r->character == c)

#endif

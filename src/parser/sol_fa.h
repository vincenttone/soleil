#ifndef _SOL_FA_H_
#define _SOL_FA_H_ 1

#include <stdio.h>
#include <stdbool.h>
#include "sol_common.h"
#include "sol_utils.h"

#define SOL_FA_CHARACTER char*
#define SOL_FA_STATE long
#define SOL_FA_STATE_NONE 0

typedef struct _SolFaRule {
	SOL_FA_STATE state;
	SOL_FA_STATE next_state;
	SOL_FA_CHARACTER character;
	int (*match)(struct _SolFaRule*, SOL_FA_STATE, SOL_FA_CHARACTER);
} SolFaRule;

SolFaRule* solFaRule_new(SOL_FA_STATE state, SOL_FA_STATE next_state, SOL_FA_CHARACTER character);
void solFaRule_free(SolFaRule *rule);

#define solFaRule_state(r) r->state
#define solFaRule_next_state(r) r->next_state
#define solFaRule_character(r) r->character

#define solFaRule_set_match_func(r, f) r->match = f
#define solFaRule_current_state(r) r->state
#define solFaRule_next_state(r) r->next_state
#define solFaRule_character(r) r->character

bool solFaRule_applies_to(SolFaRule *rule, SOL_FA_STATE state, SOL_FA_CHARACTER character);

#endif

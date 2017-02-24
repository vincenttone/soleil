#ifndef _SOL_DFA_H_
#define _SOL_DFA_H_ 1

#include <stdbool.h>
#include <string.h>
#include "sol_common.h"

#define SOL_FA_CHARACTER char
#define SOL_FA_STRING char*
#define SOL_FA_STATE unsigned long

#define SOL_FA_STATE_NONE 0

#define SOL_FA_RULE_INS_LEN 60

typedef struct SolFaRule {
	SOL_FA_STATE state;
	SOL_FA_STATE next_state;
	SOL_FA_CHARACTER character;
} SolFaRule;

typedef struct SolFaRuleBook {
	SolList *rules;
	SolListIter *iter;
} SolDfaRuleBook;

typedef struct SolDfa {
	SolDfaRuleBook *rule_book;
	SOL_FA_STATE current_state;
	SOL_FA_STATE accept_state;
} SolDfa;

SolFaRule* sol_new_fa_rule(SOL_FA_STATE state, SOL_FA_STATE next_state, SOL_FA_CHARACTER character);
void sol_destory_fa_rule(SolFaRule *rule);
SOL_FA_STATE sol_fa_rule_current_state(SolFaRule *rule);
SOL_FA_STATE sol_fa_rule_next_state(SolFaRule *rule);
SOL_FA_CHARACTER sol_fa_rule_character(SolFaRule *rule);
bool sol_fa_rule_applies_to(SolFaRule *rule, SOL_FA_STATE state, SOL_FA_CHARACTER character);
char* sol_fa_rule_inspect(SolFaRule *rule);

#define sol_fa_rule_character_match(r, c) (r->character == c)

void sol_dfa_free_inspect(char *i);

#define __sol_debug_print_fa_rule(r) do{		\
		char *__debug_i;						\
		__debug_i = sol_fa_rule_inspect(r);		\
		printf("DEBUG: %s", __debug_i);			\
		sol_dfa_free_inspect(__debug_i);		\
}while(0)

SolDfaRuleBook* sol_dfa_rule_book_new();
void sol_dfa_rule_book_release(SolDfaRuleBook *book);
SOL_FA_STATE sol_dfa_rule_book_next_state(SolDfaRuleBook *book, SOL_FA_STATE state, SOL_FA_CHARACTER character);
SolFaRule* sol_dfa_rule_book_rule_for(SolDfaRuleBook *book, SOL_FA_STATE state, SOL_FA_CHARACTER character);
char *sol_dfa_rule_book_inspect(SolDfaRuleBook *book);

#define sol_dfa_rule_book_add_rule(b, r) sol_list_add_node(b->rules, r)
#define sol_dfa_rule_book_remove_rule(b, r) sol_list_del_node(b->rules, r)
#define sol_dfa_rule_book_len(b) b->rules->len

SolDfa* sol_dfa_new(SolDfaRuleBook *rule_book, SOL_FA_STATE current_state, SOL_FA_STATE accept_state);
void sol_dfa_release(SolDfa *dfa);
bool sol_dfa_is_accepting(SolDfa *dfa);
void sol_dfa_read_character(SolDfa *dfa, SOL_FA_CHARACTER character);

#endif

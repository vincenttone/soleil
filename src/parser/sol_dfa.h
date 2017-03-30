#ifndef _SOL_DFA_H_
#define _SOL_DFA_H_ 1

#include <stdbool.h>
#include <string.h>
#include "sol_common.h"
#include "sol_fa.h"

typedef struct SolDfaRuleBook {
	SolList *rules;
	SolListIter *iter;
} SolDfaRuleBook;

typedef struct SolDfa {
	SolDfaRuleBook *rule_book;
	SOL_FA_STATE current_state;
	SOL_FA_STATE accept_state;
} SolDfa;

SolDfaRuleBook* sol_dfa_rule_book_new();
void sol_dfa_rule_book_release(SolDfaRuleBook *book);
SOL_FA_STATE sol_dfa_rule_book_next_state(SolDfaRuleBook *book, SOL_FA_STATE state, SOL_FA_CHARACTER character);
SolFaRule* sol_dfa_rule_book_rule_for(SolDfaRuleBook *book, SOL_FA_STATE state, SOL_FA_CHARACTER character);
char* sol_dfa_rule_book_inspect(SolDfaRuleBook *book);

#define sol_dfa_rule_book_add_rule(b, r) sol_list_add_node(b->rules, r)
#define sol_dfa_rule_book_remove_rule(b, r) sol_list_del_node(b->rules, r)
#define sol_dfa_rule_book_len(b) b->rules->len

SolDfa* sol_dfa_new(SolDfaRuleBook *rule_book, SOL_FA_STATE current_state, SOL_FA_STATE accept_state);
void sol_dfa_release(SolDfa *dfa);
bool sol_dfa_is_accepting(SolDfa *dfa);
void sol_dfa_read_character(SolDfa *dfa, SOL_FA_CHARACTER character);

#endif

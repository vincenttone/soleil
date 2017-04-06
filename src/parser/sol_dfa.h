#ifndef _SOL_DFA_H_
#define _SOL_DFA_H_ 1

#include <stdbool.h>
#include <string.h>
#include "sol_common.h"
#include "sol_utils.h"
#include "sol_fa.h"

typedef struct _SolDfaRuleBook {
	SolList *rules;
	SolListIter *iter;
} SolDfaRuleBook;

typedef struct _SolDfa {
	SolDfaRuleBook *rule_book;
	SOL_FA_STATE current_state;
	SOL_FA_STATE accept_state;
} SolDfa;

SolDfaRuleBook* solDfaRuleBook_new();
void solDfaRuleBook_free(SolDfaRuleBook *book);
SOL_FA_STATE solDfaRuleBook_next_state(SolDfaRuleBook *book, SOL_FA_STATE state, SOL_FA_CHARACTER character);
SolFaRule* solDfaRuleBook_rule_for(SolDfaRuleBook *book, SOL_FA_STATE state, SOL_FA_CHARACTER character);

#define solDfaRuleBook_add_rule(b, r) sol_list_add_node(b->rules, r)
#define solDfaRuleBook_remove_rule(b, r) sol_list_del_node(b->rules, r)
#define solDfaRuleBook_len(b) b->rules->len

SolDfa* solDfa_new(SolDfaRuleBook *rule_book, SOL_FA_STATE current_state, SOL_FA_STATE accept_state);
void solDfa_free(SolDfa *dfa);
bool solDfa_is_accepting(SolDfa *dfa);
void solDfa_read_character(SolDfa *dfa, SOL_FA_CHARACTER character);

#endif

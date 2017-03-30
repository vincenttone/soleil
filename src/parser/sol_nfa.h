#ifndef _SOL_NFA_H_
#define _SOL_NFA_H_ 1

#include "sol_common.h"

#define SolStateSet SolList;

typedef struct SolNfaRuleBook {
	SolList *rules;
} SolNfaRuleBook;

typedef struct SolNfa {
	SolNfaRuleBook *book;
	SolStateSet *current_state;
	SolStateSet *accept_states;
} SolNfa;

SolNfaRuleBook* sol_nfa_rule_book_new();
sol_nfa_rule_book_add_rule(SolNfaRuleBook*, SOL_FA_STATE, SOL_FA_STATE, SOL_FA_CHARACTER);


#endif

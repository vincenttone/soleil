#ifndef _SOL_NFA_H_
#define _SOL_NFA_H_ 1

#include "sol_common.h"
#include "sol_set.h"

#define SolStateSet SolSet;

typedef struct SolNfaRuleBook {
	SolList *rules;
} SolNfaRuleBook;

typedef struct SolNfa {
	SolNfaRuleBook *book;
	SolStateSet *current_state;
	SolStateSet *accept_states;
} SolNfa;

// nfa functions
SolNfa* solNfa_new();
void solNfa_free(SolNfa*);

int solNfa_accepting(SolNfa*);
void solNfa_read_character(SolNfa*, SOL_FA_CHARACTER);
void solNfa_read_string(SolNfa*, SOL_FA_STRING);
SolStateSet* solNfa_current_states(SolNfa*);

// rule book functions
SolNfaRuleBook* solNfaRuleBook_new();
void solNfaRuleBook_free(SolNfaRuleBook*);

void solNfaRuleBook_add_rule(SolNfaRuleBook*, SOL_FA_STATE, SOL_FA_STATE, SOL_FA_CHARACTER);
SolStateSet* solNfaRuleBook_follow_rules(SOL_FA_STATE, SOL_FA_CHARACTER);
SolStateSet* solNfaRuleBook_next_states(SolNfaRuleBook*, SolStateSet*, SOL_FA_CHARACTER);
void solNfaRuleBook_free_moves(SolNfaRuleBook*);

#endif

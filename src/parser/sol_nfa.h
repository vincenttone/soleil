#ifndef _SOL_NFA_H_
#define _SOL_NFA_H_ 1

#include "sol_common.h"
#include "sol_set.h"
#include "sol_fa.h"
#include "sol_utils.h"

#define SolNfaStates SolSet

#define solNfaStates_new solSet_new_and_init
#define solNfaStates_free solSet_free
#define solNfaStates_add solSet_add
#define solNfaStates_rewind solSet_rewind
#define solNfaStates_get solSet_get
#define solNfaStates_in solSet_in_set
#define solNfaStates_empty solSet_is_empty
#define solNfaStates_not_empty solSet_is_not_empty
#define solNfaStates_merge solSet_merge
#define solNfaStates_is_substates solSet_is_subset

#define solNfaStates_set_hash_func1

typedef struct SolNfaRuleBook {
	SolList *rules;
	SolListIter *iter;
} SolNfaRuleBook;

typedef struct SolNfa {
	SolNfaRuleBook *book;
	SolNfaStates *current_states;
	SolNfaStates *accept_states;
} SolNfa;

// nfa functions
SolNfa* solNfa_new();
void solNfa_free(SolNfa*);

#define solNfa_add_rule(n, s1, s2, c) solNfaRuleBook_add_rule(n->book, s1, s2, c)
#define solNfa_add_current_state(n, s) solNfaStates_add(n->current_states, s)
#define solNfa_add_accept_state(n, s) solNfaStates_add(n->accept_states, s)

int solNfa_accepting(SolNfa*, SOL_FA_STATE_PTR);
void solNfa_read_character(SolNfa*, SOL_FA_CHARACTER_PTR);
// void solNfa_read_string(SolNfa*, SOL_FA_STRING);
SolNfaStates* solNfa_current_states(SolNfa*);
SolNfa* solNfa_free_moves(SolNfa*);

// rule book functions
SolNfaRuleBook* solNfaRuleBook_new();
void solNfaRuleBook_free(SolNfaRuleBook*);

#define solNfaRuleBook_count(b) solList_len(b->rules)

SolNfaRuleBook* solNfaRuleBook_add_rule(SolNfaRuleBook*, SOL_FA_STATE, SOL_FA_STATE, SOL_FA_CHARACTER);
SolNfaStates* solNfaRuleBook_next_states(SolNfaStates*, SolNfaRuleBook*, SolNfaStates*, SOL_FA_CHARACTER_PTR);
SolNfaStates* solNfaRuleBook_free_moves(SolNfaStates*, SolNfaRuleBook*, SolNfaStates*);

SOL_FA_STATE solNfaRuleBookIter_follow_rule(SolNfaRuleBook*, SOL_FA_STATE_PTR, SOL_FA_CHARACTER_PTR);
void solNfaRuleBookIter_rewind(SolNfaRuleBook*);

#endif

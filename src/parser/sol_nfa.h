#ifndef _SOL_NFA_H_
#define _SOL_NFA_H_ 1

#include "sol_common.h"
#include "sol_set.h"
#include "sol_fa.h"

typedef struct SolNfaRuleBook {
	SolList *rules;
	SolListIter *iter;
} SolNfaRuleBook;

typedef struct SolNfa {
	SolNfaRuleBook *book;
	void *current_states;
	void *accept_states;
	void* (*s_new)();
	void (*s_free)(void*);
	void* (*s_add)(void*);
	void (*s_rewind)(void*);
	void* (*s_get)(void*);
	void (*s_in)(void*, void*);
	int (*s_empty)(void*);
	void (*s_merge)(void*, void*);
} SolNfa;

// nfa functions
SolNfa* solNfa_new();
void solNfa_free(SolNfa*);

int solNfa_accepting(SolNfa*);
void solNfa_read_character(SolNfa*, SOL_FA_CHARACTER);
// void solNfa_read_string(SolNfa*, SOL_FA_STRING);
void* solNfa_current_states(SolNfa*);

// rule book functions
SolNfaRuleBook* solNfaRuleBook_new();
void solNfaRuleBook_free(SolNfaRuleBook*);

#define solNfaRuleBook_count(b) solList_len(b->rules)

SolNfaRuleBook* solNfaRuleBook_add_rule(SolNfaRuleBook*, SOL_FA_STATE, SOL_FA_STATE, SOL_FA_CHARACTER);
void* solNfaRuleBook_next_states(SolNfaRuleBook*, void*, SOL_FA_CHARACTER);
void solNfaRuleBook_free_moves(SolNfaRuleBook*);

SOL_FA_STATE solNfaRuleBookIter_follow_rule(SOL_FA_STATE s1, SOL_FA_CHARACTER c);
void solNfaRuleBookIter_rewind(solNfaRuleBook *book)

#endif

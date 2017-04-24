#ifndef _SOL_NFA_H_
#define _SOL_NFA_H_ 1

#include "sol_common.h"
#include "sol_utils.h"
#include "sol_hash.h"
#include "sol_set.h"

typedef struct _SolNfaState {
	void *s;  // state
	SolHash *n; // next states hash
	SolSet *f; // free moves
} SolNfaState;

typedef struct _SolNfa {
	SolSet *cs; // current states
	SolSet *as; // accepting states
	SolHash *als; // all states
	SolSet *alc; // all characters
	sol_f_match_ptr f_sm; // func state match
	sol_f_match_ptr f_cm; // func character match
	sol_f_match_ptr f_psm; // func nfa state match
	sol_f_free_ptr f_sf; // func state free
	sol_f_free_ptr f_cf;  // func state free
} SolNfa;

SolNfa* solNfa_new();
void solNfa_free();
int solNfa_add_rule(SolNfa*, void*, void*, void*);
int solNfa_read_character(SolNfa *p, void* c);
int solNfa_add_current_state(SolNfa*, void*);
int solNfa_add_accepting_state(SolNfa*, void*);
int solNfa_is_accepted(SolNfa*);
int _solNfa_states_free_moves(SolSet*, SolSet*);
int solNfa_free_moves(SolNfa*);

#define solNfa_set_current_states(p, s) p->cs = s
#define solNfa_current_states(p) p->cs
#define solNfa_set_accepting_states(p, s) p->as = s
#define solNfa_accepting_states(p) p->as

#define solNfa_set_state_match_func(p, f) p->f_sm = f
#define solNfa_set_character_match_func(p, f) p->f_cm = f
#define solNfa_set_state_free_func(p, f) p->f_sf = f
#define solNfa_set_character_free_func(p, f) p->f_cf = f

SolNfaState* solNfaState_new(void*);
void solNfaState_free(SolNfaState*);
inline int solNfaState_add_rule(SolNfaState*, SolNfaState*, void*);
SolNfaState* solNfaState_next_states(SolNfaState*, void*);
#define solNfaState_free_moves(n) n->f
#define solNfaState_set_state(ps, s) ps->cs = s

void _solNfaState_free(void *ps);
void (*solNfaState_free_ptr)(void*);
int solNfaState_is_same(SolNfaState*, SolNfaState*);
int _solNfaState_is_same(void*, void*);
int (*solNfaState_is_same_ptr)(void*, void*);

size_t solNfaState_hash_func1(void*);
size_t solNfaState_hash_func2(void*);
inline SolSet* _solNfa_nfa_state_set_new(SolNfa*);

#endif

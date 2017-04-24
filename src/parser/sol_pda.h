#ifndef _SOL_PDA_H_
#define _SOL_PDA_H_ 1

#include "sol_common.h"
#include "sol_utils.h"
#include "sol_hash.h"
#include "sol_set.h"

#define SOL_PDA_IS_NONE 0
#define SOL_PDA_IS_STATE  1
#define SOL_PDA_IS_STATES  2

typedef struct _SolPdaState {
	void *s;  // state
	SolHash *n; // next states hash
	SolSet *f; // free moves
} SolPdaState;

typedef struct _SolPda {
	SolSet *cs; // current states
	SolSet *as; // accepting states
	SolHash *als; // all states
	SolSet *alc; // all characters
	sol_f_match_ptr f_sm; // func state match
	sol_f_match_ptr f_cm; // func character match
	sol_f_match_ptr f_psm; // func pda state match
	sol_f_free_ptr f_sf; // func state free
	sol_f_free_ptr f_cf;  // func state free
} SolPda;

SolPda* solPda_new();
void solPda_free();
int solPda_add_rule(SolPda*, void*, void*, void*);
int solPda_read_character(SolPda *p, void* c);
int solPda_add_current_state(SolPda*, void*);
int solPda_add_accepting_state(SolPda*, void*);
int solPda_is_accepted(SolPda*);

#define solPda_set_current_states(p, s) p->cs = s
#define solPda_current_states(p) p->cs
#define solPda_set_accepting_states(p, s) p->as = s
#define solPda_accepting_states(p) p->as

#define solPda_set_state_match_func(p, f) p->f_sm = f
#define solPda_set_character_match_func(p, f) p->f_cm = f
#define solPda_set_state_free_func(p, f) p->f_sf = f
#define solPda_set_character_free_func(p, f) p->f_cf = f

SolPdaState* solPdaState_new(void*);
void solPdaState_free(SolPdaState*);
inline int solPdaState_add_rule(SolPdaState*, SolPdaState*, void*);
SolPdaState* solPdaState_next_states(SolPdaState*, void*);
SolSet* solPdaState_free_moves(SolPdaState*);
#define solPdaState_set_state(ps, s) ps->cs = s

void _solPdaState_free(void *ps);
void (*solPdaState_free_ptr)(void*);
int solPdaState_is_same(SolPdaState*, SolPdaState*);
int _solPdaState_is_same(void*, void*);
int (*solPdaState_is_same_ptr)(void*, void*);

size_t solPdaState_hash_func1(void*);
size_t solPdaState_hash_func2(void*);

#endif

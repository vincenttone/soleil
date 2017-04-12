#ifndef _SOL_PDA_H_
#define _SOL_PDA_H_ 1

#include "sol_common.h"
#include "sol_hash.h"
#include "sol_set.h"

typedef struct _SolPdaState {
	void *s;  // state
	SolHash *n; // next states hash
	SolSet *f; // free moves
} SolPdaState;

typedef struct _SolPda {
	SolPdaState *cs; // current state
	SolHash *as; // all states
	int (*f_sm)(void*, void*); // func state match
	int (*f_cm)(void*, void*); // func character match
	void (*f_fs)(void*); // func free state
	void (*f_fc)(void*); // func free character
} SolPda;

SolPda* solPda_new();
void solPda_free();
int solPda_add_rule(SolPda*, void*, void*, void*);
void* solPda_next_states(SolPda *p, void* c);

SolPdaState* solPdaState_new(void*);
void solPdaState_free(SolPdaState*);
int solPdaState_add_rule(SolPdaState*, SolPdaState*, void*);
#define solPdaState_set_state(ps, s) ps->cs = s

#endif

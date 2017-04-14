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
	SolSet *cs; // current states
	SolHash *as; // all states
	int (*f_sm)(void*, void*); // func state match
	int (*f_cm)(void*, void*); // func character match
	void (*f_sf)(void*); // func state free
	void (*f_cf)(void*); // func character free
} SolPda;

SolPda* solPda_new();
void solPda_free();
int solPda_add_rule(SolPda*, void*, void*, void*);
int solPda_step(SolPda *p, void* c);
int solPda_add_current_state(SolPda*, void*);
#define solPda_set_current_states(p, s) p->cs = s

SolPdaState* solPdaState_new(void*);
void solPdaState_free(SolPdaState*);
inline int solPdaState_add_rule(SolPda*, SolPdaState*, SolPdaState*, void*);
int solPdaState_next_states(SolSet *s, SolPdaState *ps, void *c);
#define solPdaState_set_state(ps, s) ps->cs = s

#endif

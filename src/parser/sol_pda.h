#ifndef _SOL_PDA_H_
#define _SOL_PDA_H_ 1

#include "sol_common.h"
#include "sol_hash.h"
#include "sol_set.h"
#include "sol_util.h"

typedef struct _SolPdaState {
	void *s;  // state
	SolHash *n; // next states hash
	SolSet *f; // free moves
} SolPdaState;

typedef struct _SolPda {
	SolPdaState *s; // current state
	SolList *l; // state list
	SolListIter *li; // list iter
	int (*f_sm)(void*, void*); // func state match
	int (*f_cm)(void*, void*); // func character match
	void (*f_fs)(void*); // func free state
	void (*f_fc)(void*); // func free character
} SolPda;

SolPda* solPda_new();
void solPda_free();
int solPda_add_rule(SolPda*, void*, void*, void*);
void* solPda_next_states(SolPda *p, void* c);

SolPdaState* solPdaState_new();
void solPdaState_free(SolPdaState*);
SolPdaState* solPdaState_add_rule(SolPdaState*, void*, void*, void*);
#define solPdaState_set_state(ps, s) ps->s = s

#endif

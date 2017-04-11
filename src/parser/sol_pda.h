#ifndef _SOL_PDA_H_
#define _SOL_PDA_H_ 1

#include "sol_common.h"
#include "sol_hash.h"
#include "sol_set.h"
#include "sol_util.h"

typedef struct _SolPdaStates {
	void *s;  // state
	SolHash *n; // next states hash
	SolSet *f; // free moves
} SolPdaStates;

typedef struct _SolPda {
	SolPdaStates *s; // current state
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

SolPdaStates* solPdaStates_new();
void solPdaStates_free(SolPdaStates*);
SolPdaStates* solPdaStates_add_rule(SolPdaStates*, void*, void*, void*);
#define solPdaStates_set_state(ps, s) ps->s = s

#endif

#ifndef _SOL_PDA_H_
#define _SOL_PDA_H_ 1

#include "sol_common.h"
#include "sol_hash.h"
#include "sol_set.h"

typedef struct _SolPdaState {
	void *s;  // state
	SolHash *r; // rules
	SolSet *f; // free moves
} SolPdaState;

SolPdaState* solPdaState_new();
void solPdaState_free(SolPdaState*);
int solPda_add_rule(SolPdaState*, void*, void*, void*);

#endif

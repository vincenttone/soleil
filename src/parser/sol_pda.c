#include "sol_pda.h"

SolPdaState* solPdaState_new()
{
	SolPdaState *p = sol_alloc(sizeof(SolPda));
	p->r = sol_hash_new();
	p->f = solSet_new();
	if (p == NULL || p->r == NULL || p->f == NULL) {
		solPdaState_free(p);
		return NULL;
	}
	return p;
}

void solPdaState_free(SolPdaState *p)
{
	if (p->r) {
		sol_hash_free(p->r);
	}
	if (p->f) {
		solSet_free(p->f);
	}
	if (p) {
		sol_free(p);
	}
}

int solPda_add_rule(SolPdaState *p, void *s1, void *s2, void *c)
{
	p->s = s1;
	if (c == NULL) {
		return solSet_add(p->f, s2);
	} else {
		return sol_hash_put_key_and_val(p->r, c, s2);
	}
}

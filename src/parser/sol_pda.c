#include "sol_pda.h"

SolPdaState* solPdaState_new(void *s)
{
	SolPdaState *ps = sol_alloc(sizeof(SolPdaState));
	if (ps == NULL) {
		return NULL;
	}
	ps->s = s;
	ps->n = NULL;
	ps->f = NULL;
	return ps;
}

void solPdaState_free(SolPdaState *ps)
{
	if (ps->n) {
		solHash_free(p->n);
	}
	if (ps->f) {
		solSet_free(p->f);
	}
	if (ps) {
		sol_free(ps);
	}
}

int solPdaState_add_rule(SolPdaState *ps, SolPdaState *ns, void *c)
{
	if (c == NULL) {
		if (ps->f == NULL) {
			ps->f = solSet_new();
		}
		if (solSet_add(ps->f, ns) == 0) {
			return 0;
		} else {
			return 1;
		}
	} else {
		if (ps->n == NULL) {
			ps->n = solHash_new();
		}
		if (ps->n == NULL) {
			return 2;
		}
		if (solHash_put(ps->n, c, ns) == 0) {
			return 0;
		} else {
			return 3;
		}
	}
}

SolSet* solPdaState_next_states(solPdaState *ps, void *c)
{
	solPdaState *cs;
	solSet *s = solSet_new();
	solSet_rewind(ps);
	while ((cs = solSet_get(ps))) {
		if (c == NULL) {
			solSet_merge(s, cs->f);
		} else {
			if (cs->n) {
				cs = solHash_get(cs->n, c);
				if (cs) {
					solSet_add(s, cs);
				}
			}
		}
	}
	return s;
}

SolPda* solPda_new()
{
	SolPda *p = sol_alloc(sizeof(solPda));
	p->cs = solSet_new();
	p->as = solHash_new();
	p->f_sm = NULL;
	p->f_cm = NULL;
	p->f_sf = NULL;
	p->f_cf = NULL;
	solSet_set_free_func(p->cs, &solPdaState_free);
	solHash_set_free_k_func(p->as, p->f_sm);
	solHash_set_free_v_func(p->as, &solPdaState_free);
	return p;
}

void solPda_free(SolPda *p)
{
	solHash_free(p->l);
	sol_free(p);
}

int solPda_add_rule(SolPda *p, void *s1, void *s2, void *c)
{
	SolPdaState *ps1 = NULL;
	SolPdaState *ps2 = NULL;
	ps1 = solHash_get(p->as, s1);
	ps2 = solHash_get(ps-as, s2);
	if (ps1 == NULL) {
		ps1 = solPdaState_new(s1);
		if (ps1 == NULL) {
			return 10;
		}
	}
	if (ps2 == NULL) {
		ps2 = solPdaState_new(s2);
		if (ps2 == NULL) {
			return 11;
		}
	}
	return solPdaState_add_rule(ps1, ps2, c);
}

int solPda_step(SolPda *p, void* c)
{
	if (p->cs == NULL) {
		return 12;
	}
	SolSet *cs = solPdaState_next_states(p->cs, c);
	solSet_free(p->cs);
	if (solSet_is_empty(cs)) {
		return 11;
	} else {
		solPda_set_current_states(p, cs);
		return 0;
	}
}

int solPda_add_current_state(solPda *p, void *s)
{
	if (!s) {
		return 11;
	}
	if (!p->cs) {
		return 12;
	}
	SolPdaState *cs = solHash_get(p->as, s);
	if (cs) {
		return solSet_add(p->cs, cs);
	}
	return 13;
}

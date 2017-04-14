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
		solHash_free(ps->n);
	}
	if (ps->f) {
		solSet_free(ps->f);
	}
	if (ps) {
		sol_free(ps);
	}
}

inline int solPdaState_add_rule(SolPda *p, SolPdaState *ps, SolPdaState *ns, void *c)
{
	if (c == NULL) {
		if (ps->f == NULL) {
			ps->f = solSet_new();
		}
		if (ps->f == NULL) {
			return 1;
		}
		solSet_set_equal_func(ps->f, p->f_sm);
		return solSet_add(ps->f, ns);
	} else {
		if (ps->n == NULL) {
			ps->n = solHash_new();
		}
		if (ps->n == NULL) {
			return 2;
		}
		solHash_set_equal_func(ps->n, p->f_cm);
		return solHash_put(ps->n, c, ns);
	}
}

int solPdaState_next_states(SolSet *s, SolPdaState *ps, void *c)
{
	SolPdaState *cs;
	if (c == NULL && ps->f) {
		solSet_merge(s, ps->f);
		return 0;
	} else if (ps->n) {
		cs = solHash_get(ps->n, c);
		if (cs) {
			solSet_add(s, cs);
			return 0;
		}
	}
	return 1;
}

SolPda* solPda_new()
{
	SolPda *p = sol_alloc(sizeof(SolPda));
	p->cs = solSet_new();
	p->as = solHash_new();
	p->f_sm = NULL;
	p->f_cm = NULL;
	p->f_sf = NULL;
	p->f_cf = NULL;
	solHash_set_free_k_func(p->as, p->f_sm);
	solHash_set_free_v_func(p->as, &solPdaState_free);
	return p;
}

void solPda_free(SolPda *p)
{
	solHash_free(p->as);
	solSet_free(p->cs);
	sol_free(p);
}

int solPda_add_rule(SolPda *p, void *s1, void *s2, void *c)
{
	SolPdaState *ps1 = NULL;
	SolPdaState *ps2 = NULL;
	ps1 = solHash_get(p->as, s1);
	ps2 = solHash_get(p->as, s2);
	if (ps1 == NULL) {
		ps1 = solPdaState_new(s1);
		if (ps1 == NULL) {
			return 10;
		}
		if (solHash_put(p->as, s1, ps1) != 0) {
			return 11;
		}
	}
	if (ps2 == NULL) {
		ps2 = solPdaState_new(s2);
		if (ps2 == NULL) {
			return 10;
		}
		if (solHash_put(p->as, s2, ps2) != 0) {
			return 11;
		}
	}
	return solPdaState_add_rule(p, ps1, ps2, c);
}

int solPda_step(SolPda *p, void* c)
{
	if (p->cs == NULL) {
		return 12;
	}
	SolSet *cs = solSet_new();
	solSet_set_equal_func(cs, p->f_sm);
	SolPdaState *s;
	while ((s = solSet_get(p->cs))) {
		solPdaState_next_states(cs, s, c);
	}
	solSet_free(p->cs);
	if (solSet_is_empty(cs)) {
		return 11;
	} else {
		solPda_set_current_states(p, cs);
		return 0;
	}
}

int solPda_add_current_state(SolPda *p, void *s)
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

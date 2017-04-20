#include "sol_pda.h"

SolPdaState* solPdaState_new(void *s)
{
	SolPdaState *ps = sol_calloc(1, sizeof(SolPdaState));
	if (ps == NULL) {
		return NULL;
	}
	ps->s = s;
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

void _solPdaState_free(void *ps)
{
	solPdaState_free((SolPdaState*)ps);
}

inline int solPdaState_add_rule(SolPdaState *ps, SolPdaState *ns, void *c)
{
	if (c == NULL) {
		if (ps->f == NULL) {
			return 1;
		}
		return solSet_add(ps->f, ns);
	} else {
		if (ps->n == NULL) {
			return 2;
		}
		return solHash_put(ps->n, c, ns);
	}
}

int solPda_check_set(SolPda *p, SolSet *ns)
{
	
	return 0;
}

SolPdaState* solPdaState_next_states(SolPdaState *cs, void *c)
{
	SolPdaState *ns;
	ns = solHash_get(cs->n, c);
	return ns;
}

SolSet* solPdaState_free_moves(SolPdaState *ps)
{
	return ps->f;
}

int solPdaState_is_same(SolPdaState *s1, SolPdaState *s2)
{
	if (s1 && s2 && s1->s && s2->s) {
		if (s1->s == s2->s) {
			return 0;
		}
	}
	return 1;
}

int _solPdaState_is_same(void *s1, void *s2)
{
	return solPdaState_is_same((SolPdaState*)s1, (SolPdaState*)s2);
}

SolPda* solPda_new()
{
	solPdaState_free_ptr = &_solPdaState_free;
	SolPda *p = sol_calloc(1, sizeof(SolPda));
	// current states (ps, ...)
	p->cs = solSet_new();
	p->f_psm =  &_solPdaState_is_same;
	solSet_set_hash_func1(p->cs, &solPdaState_hash_func1);
	solSet_set_hash_func2(p->cs, &solPdaState_hash_func2);
	solSet_set_equal_func(p->cs, &p->f_psm);
	// all states {s: ps, ...}
	p->as = solHash_new();
	solHash_set_hash_func1(p->as, &sol_hash_func1);
	solHash_set_hash_func2(p->as, &sol_hash_func2);
	solHash_set_equal_func(p->as, &p->f_sm);
	solHash_set_free_k_func(p->as, &p->f_sf);
	solHash_set_free_v_func(p->as, &solPdaState_free_ptr);
	// all characters (c, ...)
	p->ac = solSet_new();
	solSet_set_hash_func1(p->ac, &sol_hash_func1);
	solSet_set_hash_func2(p->ac, &sol_hash_func2);
	solSet_set_equal_func(p->ac, &p->f_cm);
	solSet_set_free_func(p->ac, &p->f_cf);
	return p;
}

void solPda_free(SolPda *p)
{
	solHash_free(p->as);
	solSet_free(p->ac);
	solSet_free(p->cs);
	sol_free(p);
}

int solPda_add_rule(SolPda *p, void *s1, void *s2, void *c)
{
	if (c != NULL) {
		solSet_add(p->ac, c);
	}
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
	if (c == NULL && ps1->f == NULL) {
		ps1->f = solSet_new();
		if (ps1->f == NULL) {
			return 12;
		}
		solSet_set_hash_func1(ps1->f, &solPdaState_hash_func1);
		solSet_set_hash_func2(ps1->f, &solPdaState_hash_func2);
		solSet_set_equal_func(ps1->f, &p->f_psm);
	} else if (ps1->n == NULL) {
		ps1->n = solHash_new();
		solHash_set_hash_func1(ps1->n, &sol_hash_func1);
		solHash_set_hash_func2(ps1->n, &sol_hash_func2);
		solHash_set_equal_func(ps1->n, &p->f_cm);
	}
	return solPdaState_add_rule(ps1, ps2, c);
}

int solPda_read_character(SolPda *p, void* c)
{
	if (p->cs == NULL) {
		return 21;
	}
	SolSet *nss = NULL;
	SolPdaState *cs;
	SolPdaState *ns;
	SolSet *fss;
	solSet_rewind(p->cs);
	while ((cs = solSet_get(p->cs))) {
		ns = solPdaState_next_states(cs, c);
		if (ns) {
			if (nss == NULL) {
				nss = solSet_new();
				if (nss == NULL) {
					return 2;
				}
				solSet_set_hash_func1(nss, &solPdaState_hash_func1);
				solSet_set_hash_func2(nss, &solPdaState_hash_func2);
				solSet_set_equal_func(nss, &p->f_psm);
			}
			solSet_add(nss, ns);
			fss = solPdaState_free_moves(ns);
			if (fss && solSet_is_not_empty(fss)) {
				solSet_merge(nss, fss);
			}
		}
	}
	if (nss) {
		if (solSet_is_empty(nss)) {
			solSet_free(nss);
			return 1;
		}
		solSet_free(p->cs);
		solPda_set_current_states(p, nss);
	}
	return 0;
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

size_t solPdaState_hash_func1(void *ps)
{
	void *s = ((SolPdaState*)ps)->s;
	return sol_hash_func1(s);
}

size_t solPdaState_hash_func2(void *ps)
{
	void *s = ((SolPdaState*)ps)->s;
	return sol_hash_func2(s);
}

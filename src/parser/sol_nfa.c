#include "sol_nfa.h"

SolNfaState* solNfaState_new(void *s)
{
	SolNfaState *ps = sol_calloc(1, sizeof(SolNfaState));
	if (ps == NULL) {
		return NULL;
	}
	ps->s = s;
	return ps;
}

void solNfaState_free(SolNfaState *ps)
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

void _solNfaState_free(void *ps)
{
	solNfaState_free((SolNfaState*)ps);
}

inline int solNfaState_add_rule(SolNfaState *ps, SolNfaState *ns, void *c)
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

SolNfaState* solNfaState_next_states(SolNfaState *cs, void *c)
{
	SolNfaState *ns;
	ns = solHash_get(cs->n, c);
	return ns;
}

int _solNfa_states_free_moves(SolSet *tss, SolSet *css)
{
	if (css == NULL) {
		return 0;
	}
	size_t l1, l2;
	SolNfaState *cs;
	SolSet *fss;
	solSet_rewind(css);
	while ((cs = solSet_get(css))) {
		fss = solNfaState_free_moves(cs);
		if (fss == NULL) continue;
		l1 = solSet_count(tss);
		solSet_merge(tss, fss);
		l2 = solSet_count(tss);
		if (l1 != l2) {
			_solNfa_states_free_moves(tss, fss);
		}
	}
	return 0;
}

int solNfa_free_moves(SolNfa *nfa)
{
	return _solNfa_states_free_moves(nfa->cs, nfa->cs);
}

int solNfaState_is_same(SolNfaState *s1, SolNfaState *s2)
{
	if (s1 && s2 && s1->s && s2->s) {
		if (s1->s == s2->s) {
			return 0;
		}
	}
	return 1;
}

int _solNfaState_is_same(void *s1, void *s2)
{
	return solNfaState_is_same((SolNfaState*)s1, (SolNfaState*)s2);
}

SolNfa* solNfa_new()
{
	solNfaState_free_ptr = &_solNfaState_free;
	SolNfa *p = sol_calloc(1, sizeof(SolNfa));
	p->f_psm =  &_solNfaState_is_same;
	// current states (ps, ...)
	p->cs = solSet_new();
	solSet_set_hash_func1(p->cs, &solNfaState_hash_func1);
	solSet_set_hash_func2(p->cs, &solNfaState_hash_func2);
	solSet_set_equal_func(p->cs, &p->f_psm);
	// accpetings states (ps, ...)
	p->as = solSet_new();
	solSet_set_hash_func1(p->as, &solNfaState_hash_func1);
	solSet_set_hash_func2(p->as, &solNfaState_hash_func2);
	solSet_set_equal_func(p->as, &p->f_psm);
	// all states {s: ps, ...}
	p->als = solHash_new();
	solHash_set_hash_func1(p->als, &sol_hash_func1);
	solHash_set_hash_func2(p->als, &sol_hash_func2);
	solHash_set_equal_func(p->als, &p->f_sm);
	solHash_set_free_k_func(p->als, &p->f_sf);
	solHash_set_free_v_func(p->als, &solNfaState_free_ptr);
	// all characters (c, ...)
	p->alc = solSet_new();
	solSet_set_hash_func1(p->alc, &sol_hash_func1);
	solSet_set_hash_func2(p->alc, &sol_hash_func2);
	solSet_set_equal_func(p->alc, &p->f_cm);
	solSet_set_free_func(p->alc, &p->f_cf);
	return p;
}

void solNfa_free(SolNfa *p)
{
	solHash_free(p->als);
	solSet_free(p->alc);
	solSet_free(p->cs);
	solSet_free(p->as);
	sol_free(p);
}

int solNfa_is_accepted(SolNfa *p)
{
	return solSet_has_intersection(p->cs, p->as);
}

int solNfa_add_rule(SolNfa *p, void *s1, void *s2, void *c)
{
	if (c != NULL) {
		solSet_add(p->alc, c);
	}
	SolNfaState *ps1 = NULL;
	SolNfaState *ps2 = NULL;
	ps1 = solHash_get(p->als, s1);
	ps2 = solHash_get(p->als, s2);
	if (ps1 == NULL) {
		ps1 = solNfaState_new(s1);
		if (ps1 == NULL) {
			return 10;
		}
		if (solHash_put(p->als, s1, ps1) != 0) {
			return 11;
		}
	}
	if (ps2 == NULL) {
		ps2 = solNfaState_new(s2);
		if (ps2 == NULL) {
			return 10;
		}
		if (solHash_put(p->als, s2, ps2) != 0) {
			return 11;
		}
	}
	if (c == NULL && ps1->f == NULL) {
		ps1->f = solSet_new();
		if (ps1->f == NULL) {
			return 12;
		}
		solSet_set_hash_func1(ps1->f, &solNfaState_hash_func1);
		solSet_set_hash_func2(ps1->f, &solNfaState_hash_func2);
		solSet_set_equal_func(ps1->f, &p->f_psm);
	} else if (ps1->n == NULL) {
		ps1->n = solHash_new();
		solHash_set_hash_func1(ps1->n, &sol_hash_func1);
		solHash_set_hash_func2(ps1->n, &sol_hash_func2);
		solHash_set_equal_func(ps1->n, &p->f_cm);
	}
	return solNfaState_add_rule(ps1, ps2, c);
}

inline SolSet* _solNfa_nfa_state_set_new(SolNfa *p)
{
	SolSet *ss = solSet_new();
	if (!ss) {
		return NULL;
	}
	solSet_set_hash_func1(ss, &solNfaState_hash_func1);
	solSet_set_hash_func2(ss, &solNfaState_hash_func2);
	solSet_set_equal_func(ss, &p->f_psm);
	return ss;
}

int solNfa_read_character(SolNfa *p, void* c)
{
	if (p->cs == NULL) {
		return 21;
	}
	SolSet *nss = NULL;
	SolNfaState *cs;
	SolNfaState *ns;
	solSet_rewind(p->cs);
	while ((cs = solSet_get(p->cs))) {
		ns = solNfaState_next_states(cs, c);
		if (ns) {
			if (nss == NULL) {
				nss = _solNfa_nfa_state_set_new(p);
				if (nss == NULL) {
					return 22;
				}
			}
			solSet_add(nss, ns);
		}
	}
	if (nss) {
		if (solSet_is_empty(nss)) {
			solSet_free(nss);
			return 1;
		}
		solSet_free(p->cs);
		solNfa_set_current_states(p, nss);
	} else if (solSet_is_not_empty(p->cs)) {
		nss = _solNfa_nfa_state_set_new(p);
		if (nss == NULL) {
			return 22;
		}
		solSet_free(p->cs);
		solNfa_set_current_states(p, nss);
	}
	solNfa_free_moves(p);
	return 0;
}

int solNfa_add_current_state(SolNfa *p, void *s)
{
	if (!s) {
		return 11;
	}
	if (!p->cs) {
		return 12;
	}
	SolNfaState *cs = solHash_get(p->als, s);
	if (cs) {
		if (solSet_add(p->cs, cs) == 0) {
			return solNfa_free_moves(p);
		}
	}
	return 13;
}

int solNfa_add_accepting_state(SolNfa *p, void *s)
{
	if (!s) {
		return 11;
	}
	if (!p->as) {
		return 12;
	}
	SolNfaState *cs = solHash_get(p->als, s);
	if (cs) {
		return solSet_add(p->as, cs);
	}
	return 13;
}

size_t solNfaState_hash_func1(void *ps)
{
	void *s = ((SolNfaState*)ps)->s;
	return sol_hash_func1(s);
}

size_t solNfaState_hash_func2(void *ps)
{
	void *s = ((SolNfaState*)ps)->s;
	return sol_hash_func2(s);
}

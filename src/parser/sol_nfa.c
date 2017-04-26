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
	if (solNfaState_next_state_table(ps)) {
		solHash_free(solNfaState_next_state_table(ps));
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

inline int solNfaState_add_rule(SolNfaState *cs, void *ns, void *c)
{
	if (c == NULL) {
		if (cs->f == NULL) {
			return 1;
		}
		return solSet_add(cs->f, ns);
	} else {
		if (solNfaState_next_state_table(cs) == NULL) {
			return 2;
		}
		return solHash_put(solNfaState_next_state_table(cs), c, ns);
	}
}

void* solNfaState_next_state(SolNfaState *cs, void *c)
{
	if (solNfaState_next_state_table(cs) == NULL) {
		return NULL;
	}
	void *ns = solHash_get(solNfaState_next_state_table(cs), c);
	return ns;
}

int _solNfa_states_free_moves(SolNfa *nfa, SolSet *tss, SolSet *css)
{
	if (css == NULL) {
		return 0;
	}
	size_t l1, l2;
	void *cs;
	SolNfaState *cns;
	SolSet *fss;
	solSet_rewind(css);
	while ((cs = solSet_get(css))) {
		cns = solNfa_conv_nfa_state(nfa, cs);
		if (cns == NULL) {
			return 1;
		}
		fss = solNfaState_free_moves_set(cns);
		if (fss == NULL) continue;
		l1 = solSet_count(tss);
		solSet_merge(tss, fss);
		l2 = solSet_count(tss);
		if (l1 != l2) {
			_solNfa_states_free_moves(nfa, tss, fss);
		}
	}
	return 0;
}

int solNfa_free_moves(SolNfa *nfa)
{
	return _solNfa_states_free_moves(nfa, nfa->cs, nfa->cs);
}

SolNfa* solNfa_new()
{
	SolNfa *nfa = sol_calloc(1, sizeof(SolNfa));
	nfa->ss = solSet_new();
	nfa->cs = solSet_new();
	nfa->as = solSet_new();
	nfa->als = solHash_new();
	return nfa;
}

void solNfa_init(SolNfa *nfa)
{
	solSet_set_hash_func1(solNfa_starting_states(nfa), solNfa_state_hash_func1(nfa));
	solSet_set_hash_func2(solNfa_starting_states(nfa), solNfa_state_hash_func2(nfa));
	solSet_set_equal_func(solNfa_starting_states(nfa), solNfa_state_match_func(nfa));

	solSet_set_hash_func1(solNfa_current_states(nfa), solNfa_state_hash_func1(nfa));
	solSet_set_hash_func2(solNfa_current_states(nfa), solNfa_state_hash_func2(nfa));
	solSet_set_equal_func(solNfa_current_states(nfa), solNfa_state_match_func(nfa));

	solSet_set_hash_func1(solNfa_accepting_states(nfa), solNfa_state_hash_func1(nfa));
	solSet_set_hash_func2(solNfa_accepting_states(nfa), solNfa_state_hash_func2(nfa));
	solSet_set_equal_func(solNfa_accepting_states(nfa), solNfa_state_match_func(nfa));

	solHash_set_hash_func1(solNfa_all_states(nfa), solNfa_state_hash_func1(nfa));
	solHash_set_hash_func2(solNfa_all_states(nfa), solNfa_state_hash_func2(nfa));
	solHash_set_equal_func(solNfa_all_states(nfa), solNfa_state_match_func(nfa));
	solHash_set_free_v_func(solNfa_all_states(nfa), &_solNfaState_free);
}

void solNfa_free(SolNfa *p)
{
	if (p->ss) {
		solSet_free(p->ss);
	}
	if (p->als) {
		solHash_free(p->als);
	}
	if (p->cs) {
		solSet_free(p->cs);
	}
	if (p->as) {
		solSet_free(p->as);
	}
	if (p) {
		sol_free(p);
	}
}

int solNfa_is_accepted(SolNfa *p)
{
	return solSet_has_intersection(p->cs, p->as);
}

SolNfaState* solNfa_register_state(SolNfa *nfa, void *s)
{
	SolNfaState *ns = solNfaState_new(s);
	if (ns == NULL) {
		return NULL;
	}
	if (solHash_put(nfa->als, s, ns) != 0) {
		solNfaState_free(ns);
		return NULL;
	}
	return ns;
}

int solNfa_add_rule(SolNfa *p, void *s1, void *s2, void *c)
{
	SolNfaState *ps1 = NULL;
	SolNfaState *ps2 = NULL;
	ps1 = solNfa_conv_nfa_state(p, s1);
	ps2 = solNfa_conv_nfa_state(p, s2);
	if (ps1 == NULL) {
		ps1 = solNfa_register_state(p, s1);
		if (ps1 == NULL) {
			return 1;
		}
	}
	if (ps2 == NULL) {
		ps2 = solNfa_register_state(p, s2);
		if (ps2 == NULL) {
			return 2;
		}
	}
	if (c == NULL && ps1->f == NULL) {
		ps1->f = _solNfa_nfa_state_set_new(p);
		if (ps1->f == NULL) {
			return 12;
		}
	} else if (solNfaState_next_state_table(ps1) == NULL) {
		solNfaState_set_next_state_table(ps1, solHash_new());
		solHash_set_hash_func1(solNfaState_next_state_table(ps1), solNfa_character_hash_func1(p));
		solHash_set_hash_func2(solNfaState_next_state_table(ps1), solNfa_character_hash_func2(p));
		solHash_set_equal_func(solNfaState_next_state_table(ps1), solNfa_character_match_func(p));
	}
	return solNfaState_add_rule(ps1, s2, c);
}

inline SolSet* _solNfa_nfa_state_set_new(SolNfa *nfa)
{
	SolSet *ss = solSet_new();
	if (!ss) {
		return NULL;
	}
	solSet_set_hash_func1(ss, solNfa_state_hash_func1(nfa));
	solSet_set_hash_func2(ss, solNfa_state_hash_func2(nfa));
	solSet_set_equal_func(ss, solNfa_state_match_func(nfa));
	return ss;
}

int solNfa_read_character(SolNfa *p, void* c)
{
	if (p->cs == NULL) {
		return 21;
	}
	if (solSet_is_empty(p->cs)) {
		return 1; // stucked
	}
	SolSet *nss = NULL;
	void *cs;
	void *ns;
	SolNfaState *cns;
	solSet_rewind(p->cs);
	while ((cs = solSet_get(p->cs))) {
		cns = solNfa_conv_nfa_state(p, cs);
		if (cns == NULL) {
			return 23;
		}
		ns = solNfaState_next_state(cns, c);
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
		solSet_free(p->cs);
		solNfa_set_current_states(p, nss);
		if (solSet_is_empty(p->cs)) {
			return 1; // stuck
		}
	} else if (solSet_is_not_empty(p->cs)) {
		nss = _solNfa_nfa_state_set_new(p);
		if (nss == NULL) {
			return 22;
		}
		solSet_free(p->cs);
		solNfa_set_current_states(p, nss);
		return 1; // stuck
	}
	solNfa_free_moves(p);
	return 0;
}

int solNfa_reset(SolNfa *nfa)
{
	if (solNfa_starting_states(nfa) == NULL) {
		return 1;
	}
	void *s;
	if (solNfa_current_states(nfa)) {
		if (solSet_is_not_empty(solNfa_current_states(nfa))) {
			solSet_free(solNfa_current_states(nfa));
		} else {
			goto build_cs;
		}
	}
	SolSet *nss = _solNfa_nfa_state_set_new(nfa);
	if (nss == NULL) {
		return 22;
	}
	solNfa_set_current_states(nfa, nss);
 build_cs:
	solSet_rewind(solNfa_starting_states(nfa));
	while ((s = solSet_get(solNfa_starting_states(nfa)))) {
		solSet_add(solNfa_current_states(nfa), s);
	}
	return 0;
}

int solNfa_add_starting_state(SolNfa *p, void *s)
{
	if (!s) {
		return 1;
	}
	if (!p->cs) {
		return 2;
	}
	if (solHash_has_key(solNfa_all_states(p), s) != 0) {
		if (solNfa_register_state(p, s) == NULL) {
			return 3;
		}
	}
	if (solSet_add(solNfa_starting_states(p), s) == 0) {
		if (solNfa_free_moves(p) == 0) {
			return 0;
		} else {
			return 4;
		}
	}
	return 5;
}

int solNfa_add_current_state(SolNfa *p, void *s)
{
	if (s == NULL) {
		return 1;
	}
	if (solNfa_current_states(p) == NULL) {
		return 2;
	}
	if (solHash_has_key(solNfa_all_states(p), s) != 0) {
		return 3;
	}
	if (solSet_add(solNfa_current_states(p), s) == 0) {
		if (solNfa_free_moves(p) == 0) {
			return 0;
		} else {
			return 4;
		}
	}
	return 5;
}

int solNfa_add_accepting_state(SolNfa *p, void *s)
{
	if (!s) {
		return 11;
	}
	if (!p->as) {
		return 12;
	}
	if (solHash_has_key(solNfa_all_states(p), s) != 0) {
		if (solNfa_register_state(p, s) == NULL) {
			return 13;
		}
	}
	return solSet_add(p->as, s);
}

int solNfa_add_accepting_states(SolNfa *n, SolSet *ss)
{
	SolNfaState *s;
	solSet_rewind(ss);
	while ((s = solSet_get(ss))) {
		if (solNfa_add_accepting_state(n, s) != 0) {
			return 1;
		}
	}
	return 0;
}

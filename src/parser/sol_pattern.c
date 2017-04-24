#include "sol_pattern.h"

SolPattern* solPattern_new()
{
	SolPattern *p = sol_alloc(sizeof(SolPattern));
	if (p == NULL) {
		return NULL;
	}
	p->c = 1;
	p->l = solList_new();
	if (p->l == NULL) {
		sol_free(p);
	}
	return p;
}

void solPattern_free(SolPattern *p)
{
	if (p->l) {
		sol_free(p->l);
	}
	if (p) {
		sol_free(p);
	}
}

SolVal* solPattern_gen_state(SolPattern *p)
{
	SolVal *s = sol_alloc(sizeof(SolVal));
	solVal_int(s, p.c++);
	return s;
}

SolNfa* solPattern_gen_nfa()
{
	SolNfa *nfa = solNfa_new();
	if (nfa == NULL) {
		return NULL;
	}
	solNfa_set_state_free_func(nfa, &sol_free);
	solNfa_set_character_free_func(nfa, &sol_free);
	solNfa_set_state_match_func(nfa, &_solPattern_val_equal);
	solNfa_set_character_match_func(nfa, &_solPattern_val_equal);
	return nfa;
}

SolNfa* solPattern_empty_new(SolPattern *p)
{
	SolNfa *nfa = solPattern_gen_nfa();
	if (nfa == NULL) {
		return NULL;
	}
	SolVal *v = solPatter_gen_state(p);
	if (v == NULL) {
		solNfa_free(nfa);
		return NULL;
	}
	solNfa_add_current_state(v);
	solNfa_add_accepting_state(v);
	return nfa;
}

SolPattern* solPattern_literal_new(SolPattern *p, SolVal *c)
{
	SolVal *s1 = solPatter_gen_state(p);
	if (s1 == NULL) {
		return NULL;
	}
	SolVal *s2 = solPatter_gen_state(p);
	if (s2 == NULL) {
		sol_free(s1);
		return NULL;
	}
	SolNfa *nfa = solPattern_gen_nfa();
	if (nfa == NULL) {
		sol_free(s1);
		sol_free(s2);
		return NULL;
	}
	solNfa_add_rule(nfa, s1, s2, c);
	solNfa_add_current_state(nfa, s1);
	solNfa_add_accepting_state(nfa, s2);
	return nfa;
}

SolPattern* solPattern_concatenate_new(SolPattern *p, SolVal *c1, SolVal *c2)
{
	SolVal *s1 = solPatter_gen_state(p);
	if (s1 == NULL) {
		return NULL;
	}
	SolVal *s2 = solPatter_gen_state(p);
	if (s2 == NULL) {
		sol_free(s1);
		return NULL;
	}
	SolVal *s3 = solPatter_gen_state(p);
	if (s2 == NULL) {
		sol_free(s1);
		sol_free(s2);
		return NULL;
	}
	SolNfa *nfa = solPattern_gen_nfa();
	if (nfa == NULL) {
		sol_free(s1);
		sol_free(s2);
		sol_free(s3);
		return NULL;
	}
	solNfa_add_rule(nfa, s1, s2, c1);
	solNfa_add_rule(nfa, s2, s3, c2);
	solNfa_add_current_state(nfa, s1);
	solNfa_add_accepting_state(nfa, s3);
	return nfa;
}

SolPattern* solPattern_choose_new(SolPattern *p, SolVal *c1, SolVal *c2)
{
	SolVal *s1 = solPatter_gen_state(p);
	if (s1 == NULL) {
		return NULL;
	}
	SolVal *s2 = solPatter_gen_state(p);
	if (s2 == NULL) {
		sol_free(s1);
		return NULL;
	}
	SolNfa *nfa = solPattern_gen_nfa();
	if (nfa == NULL) {
		sol_free(s1);
		sol_free(s2);
		return NULL;
	}
	solNfa_add_rule(nfa, s1, s2, c1);
	solNfa_add_rule(nfa, s1, s2, c2);
	solNfa_add_current_state(nfa, s1);
	solNfa_add_accepting_state(nfa, s2);
	return nfa;
}

SolNfa* solPattern_repeat_new(SolPattern *p, SolVal *c)
{
	SolVal *s = solPatter_gen_state(p);
	if (s == NULL) {
		return NULL;
	}
	SolNfa *nfa = solPattern_gen_nfa();
	if (nfa == NULL) {
		sol_free(s);
		return NULL;
	}
	solNfa_add_rule(nfa, s, s, c);
	solNfa_add_current_state(nfa, s);
	solNfa_add_accepting_state(nfa, s);
	return nfa;
}

int _solPattern_val_equal(void *k1, void *k2)
{
	return solVal_equal((SolVal*)k1, (SolVal*)k2);
}

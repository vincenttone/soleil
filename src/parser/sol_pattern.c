#include "sol_pattern.h"

SolPattern* solPattern_new()
{
	SolPattern *p = sol_alloc(sizeof(SolPattern));
	if (p == NULL) {
		return NULL;
	}
	p->nfa = solPattern_gen_nfa();
	return p;
}

void solPattern_free(SolPattern *p)
{
	if (p->nfa) {
		solNfa_free(p->nfa);
	}
	if (p) {
		sol_free(p);
	}
}

int solPattern_match(SolPattern *p, SolVal *s)
{
	size_t i = 0;
	char *str = solVal_str_val(s);
	if (solVal_is_str(s)) {
		while (i++ < solVal_get_size(s)) {
			solNfa_read_character(p->nfa, str + i);
		}
	}
	return solNfa_is_accepted(p->nfa);
}

struct _SolPatternStateGen* solPatternStateGen_new()
{
	struct _SolPatternStateGen *g = sol_alloc(sizeof(struct _SolPatternStateGen));
	g->i = 1;
	return g;
}

SolVal* solPattern_gen_state(struct _SolPatternStateGen *gen)
{
	SolVal *s = sol_alloc(sizeof(SolVal));
	solVal_int(s, gen->i++);
	return s;
}

inline SolNfa* solPattern_gen_nfa()
{
	SolNfa *nfa = solNfa_new();
	if (nfa == NULL) {
		return NULL;
	}
	solNfa_set_state_free_func(nfa, &sol_free);
	//solNfa_set_character_free_func(nfa, &sol_free);
	solNfa_set_state_match_func(nfa, &_solPattern_val_equal);
	solNfa_set_character_match_func(nfa, &_solPattern_char_equal);
	return nfa;
}

SolPattern* solPattern_empty_new(struct _SolPatternStateGen *gen)
{
	SolPattern *p = solPattern_new();
	if (p == NULL) {
		return NULL;
	}
	SolVal *s = solPattern_gen_state(gen);
	if (s == NULL) {
		solPattern_free(p);
		return NULL;
	}
	solNfa_add_current_state(p->nfa, s);
	solNfa_add_accepting_state(p->nfa, s);
	return p;
}

SolPattern* solPattern_literal_new(struct _SolPatternStateGen *gen, void *c)
{
	SolVal *s1 = solPattern_gen_state(gen);
	if (s1 == NULL) {
		return NULL;
	}
	SolVal *s2 = solPattern_gen_state(gen);
	if (s2 == NULL) {
		sol_free(s1);
		return NULL;
	}
	SolPattern *p = solPattern_new();
	if (p == NULL) {
		sol_free(s1);
		sol_free(s2);
		return NULL;
	}
	solNfa_add_rule(p->nfa, s1, s2, c);
	solNfa_add_current_state(p->nfa, s1);
	solNfa_add_accepting_state(p->nfa, s2);
	return p;
}

int solPattern_concatenate(SolPattern *p1, SolPattern *p2)
{
	if (solHash_merge(solNfa_all_states(p1->nfa), solNfa_all_states(p2->nfa)) != 0) {
		return 1;
	}
	if (solSet_merge(solNfa_all_characters(p1->nfa), solNfa_all_characters(p2->nfa)) != 0) {
		return 1;
	}
	SolNfaState *s1;
	SolNfaState *s2;
	while ((s1 = solSet_get(solNfa_accepting_states(p1->nfa)))) {
		while ((s2 = solSet_get(solNfa_accepting_states(p2->nfa)))) {
			solNfaState_add_rule(s1, s2, NULL);
		}
	}
	solNfa_set_accepting_states(p1->nfa, solNfa_accepting_states(p2->nfa));
	solNfa_set_all_states(p2->nfa, NULL);
	solNfa_set_all_characters(p2->nfa, NULL);
	solPattern_free(p2);
	return 0;
}

int solPattern_choose(SolPattern *p1, SolPattern *p2)
{
	int m;
	m = solHash_merge(solNfa_all_states(p1->nfa), solNfa_all_states(p2->nfa));
	if (m != 0) {
		return 1;
	}
	m = solSet_merge(solNfa_all_characters(p1->nfa), solNfa_all_characters(p2->nfa));
	if (m != 0) {
		return 1;
	}
	SolNfaState *s1;
	SolNfaState *s2;
	while ((s1 = solSet_get(solNfa_current_states(p1->nfa)))) {
		while ((s2 = solSet_get(solNfa_current_states(p2->nfa)))) {
			solNfaState_add_rule(s1, s2, NULL);
		}
	}
	solNfa_add_accepting_states(p1->nfa, solNfa_accepting_states(p2->nfa));
	solNfa_set_all_states(p2->nfa, NULL);
	solNfa_set_all_characters(p2->nfa, NULL);
	solPattern_free(p2);
	return 0;
}

int solPattern_repeat(SolPattern *p)
{
	SolNfaState *cs;
	SolNfaState *as;
	while ((cs = solSet_get(solNfa_current_states(p->nfa)))) {
		while ((as = solSet_get(solNfa_accepting_states(p->nfa)))) {
			solNfaState_add_rule(as, cs, NULL);
		}
	}
	return 0;
}

int _solPattern_val_equal(void *k1, void *k2)
{
	return solVal_equal((SolVal*)k1, (SolVal*)k2);
}

int _solPattern_char_equal(void *c1, void *c2)
{
	return (char*)c1 == (char*)c2;
}

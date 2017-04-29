#include <string.h>
#include <stdio.h>
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

int solPattern_reset(SolPattern *p)
{
	return solNfa_reset(p->nfa);
}

int solPattern_match(SolPattern *p, char *s, size_t size)
{
	size_t i = 0;
	int r;
	char st[] = {"x"};
	solPattern_reset(p);
	while (i < size) {
		strncpy(st, (s+i), sizeof(char));
		r = solNfa_read_character(p->nfa, (char*)st);
		if (r == 1) {
			return 1;
		} else if (r != 0){
			return r;
		}
		i++;
	}
	if (solNfa_is_accepted(p->nfa) == 0) {
		return 0;
	}
	return 2;
}

SolPatternStateGen* solPatternStateGen_new()
{
	SolPatternStateGen *g = sol_alloc(sizeof(SolPatternStateGen));
	g->i = 0;
	return g;
}

void solPatternStateGen_free(SolPatternStateGen *g)
{
	sol_free(g);
}

SolPatternState* solPattern_gen_state(SolPatternStateGen *g)
{
	g->i++;
	g->l[g->i] = g->i;
	return &(g->l[g->i]);
}

inline SolNfa* solPattern_gen_nfa()
{
	SolNfa *nfa = solNfa_new();
	if (nfa == NULL) {
		return NULL;
	}
	solNfa_set_state_match_func(nfa, &_solPattern_state_equal);
	solNfa_set_character_match_func(nfa, &_solPattern_char_equal);
	solNfa_set_state_hash_func1(nfa, &sol_i_hash_func1);
	solNfa_set_state_hash_func2(nfa, &sol_i_hash_func2);
	solNfa_set_character_hash_func1(nfa, &sol_c_hash_func1);
	solNfa_set_character_hash_func2(nfa, &sol_c_hash_func2);
	solNfa_init(nfa);
	return nfa;
}

SolPattern* solPattern_empty_new(SolPatternStateGen *gen)
{
	SolPattern *p = solPattern_new();
	if (p == NULL) {
		return NULL;
	}
	SolPatternState *s = solPattern_gen_state(gen);
	solNfa_add_starting_state(p->nfa, s);
	solNfa_add_accepting_state(p->nfa, s);
	return p;
}

SolPattern* solPattern_literal_new(SolPatternStateGen *gen, void *c)
{
	SolPatternState *s1 = solPattern_gen_state(gen);
	SolPatternState *s2 = solPattern_gen_state(gen);
	SolPattern *p = solPattern_new();
	if (p == NULL) {
		return NULL;
	}
	solNfa_add_rule(p->nfa, s1, s2, c);
	solNfa_add_starting_state(p->nfa, s1);
	solNfa_add_accepting_state(p->nfa, s2);
	return p;
}

SolPattern* solPattern_concatenate(SolPattern *p1, SolPattern *p2)
{
	if (solHash_merge(solNfa_all_states(p1->nfa), solNfa_all_states(p2->nfa)) != 0) {
		solPattern_free(p2);
		return NULL;
	}
	SolPatternState *s1;
	SolPatternState *s2;
	solSet_rewind(solNfa_accepting_states(p1->nfa));
	solSet_rewind(solNfa_starting_states(p2->nfa));
	while ((s1 = solSet_get(solNfa_accepting_states(p1->nfa)))) {
		while ((s2 = solSet_get(solNfa_starting_states(p2->nfa)))) {
			solNfa_add_rule(p1->nfa, s1, s2, NULL);
		}
	}
	solNfa_dup_accepting_states(p1->nfa, p2->nfa);
	solNfa_wipe_all_states(p2->nfa);
	solPattern_free(p2);
	return p1;
}

SolPattern* solPattern_choose(SolPattern *p1, SolPattern *p2)
{
	int m;
	m = solHash_merge(solNfa_all_states(p1->nfa), solNfa_all_states(p2->nfa));
	if (m != 0) {
		return NULL;
	}
	SolPatternState *s1;
	SolPatternState *s2;
	solSet_rewind(solNfa_starting_states(p1->nfa));
	solSet_rewind(solNfa_starting_states(p2->nfa));
	while ((s1 = solSet_get(solNfa_starting_states(p1->nfa)))) {
		while ((s2 = solSet_get(solNfa_starting_states(p2->nfa)))) {
			solNfa_add_rule(p1->nfa, s1, s2, NULL);
		}
	}
	solNfa_add_accepting_states(p1->nfa, solNfa_accepting_states(p2->nfa));
	solNfa_wipe_all_states(p2->nfa);
	solPattern_free(p2);
	return p1;
}

SolPattern* solPattern_repeat(SolPattern *p)
{
	SolPatternState *cs;
	SolPatternState *as;
	solSet_rewind(solNfa_accepting_states(p->nfa));
	solSet_rewind(solNfa_starting_states(p->nfa));
	while ((as = solSet_get(solNfa_accepting_states(p->nfa)))) {
		while ((cs = solSet_get(solNfa_starting_states(p->nfa)))) {
			solNfa_add_rule(p->nfa, as, cs, NULL);
		}
	}
	return p;
}

int _solPattern_state_equal(void *s1, void *s2)
{
	if ((int*)s1 == (int*)s2) {
		return 0;
	}
	return 1;
}

int _solPattern_char_equal(void *c1, void *c2)
{
	if (*(char*)c1 == *(char*)c2) {
		return 0;
	}
	return 1;
}

void _solPattern_debug_relations(SolPattern *p)
{
	int *m;
	solSet_rewind(solNfa_starting_states(p->nfa));
	printf("Starting states:\n");
	while ((m = solSet_get(solNfa_starting_states(p->nfa)))) {
		printf("State: (%d):\n", *(int*)m);
	}
	solSet_rewind(solNfa_accepting_states(p->nfa));
	printf("Accepting states:\n");
	while ((m = solSet_get(solNfa_accepting_states(p->nfa)))) {
		printf("State: (%d):\n", *(int*)m);
	}
	printf("All states:\n");
	int *n;
	SolHashIter *i = solHashIter_new(solNfa_all_states(p->nfa));
	SolHashIter *j;
	SolNfaState *s;
	SolHashRecord *r;
	solHashIter_rewind(i);
	while ((r = solHashIter_get(i))) {
		// printf("STATE: (%d):\n", *(int*)r->k);
		n = (int*)r->k;
		s = r->v;
		if (s->n) {
			j = solHashIter_new(s->n);
			solHashIter_rewind(j);
			while ((r = solHashIter_get(j))) {
				printf("rules: (%d) -(%s)-> (%d)\n", *n, (char*)r->k, *(int*)r->v);
			}
			solHashIter_free(j);
		}
		if (s->f) {
			solSet_rewind(s->f);
			while ((m = solSet_get(s->f))) {
				printf("free moves: (%d) -> (%d)\n", *n, *m);
			}
		}
	}
	solHashIter_free(i);
}

#include "sol_pattern_dfa.h"

SolPattern* solPattern_new()
{
	SolPattern *p = sol_calloc(1, sizeof(SolPattern));
	if (p == NULL) {
		return NULL;
	}
	if (solPattern_dfa(p) == NULL) {
		p->dfa = solDfa_new(&sol_i_hash_func1, &sol_i_hash_func2, &_solPattern_state_equal,
							&sol_c_hash_func1, &sol_c_hash_func2, &_solPattern_char_equal);
	}
	if (solPattern_dfa(p) == NULL) {
		solPattern_free(p);
		return NULL;
	}
	return p;
}

void solPattern_free(SolPattern *p)
{
	if (p->dfa) {
		solDfa_free(p->dfa);
	}
	if (p->s) {
		solStack_free(p->s);
	}
	if (p) {
		sol_free(p);
	}
}

int solPattern_push_state(SolPattern *p, SolPatternState *s)
{
	if (solPattern_state_stack(p) == NULL) {
		p->s = solStack_new();
	}
	if (solPattern_state_stack(p) == NULL) {
		return 1;
	}
	if (solStack_push(solPattern_state_stack(p), s)) {
		return 0;
	}
	return 2;
}

SolPatternState* solPattern_pop_state(SolPattern *p)
{
	if (solPattern_state_stack(p) == NULL) {
		return NULL;
	}
	SolPatternState *s = (SolPatternState*)(solStack_pop(solPattern_state_stack(p)));
	return s;
}

SolPatternStateGen* solPatternStateGen_new()
{
	SolPatternStateGen *g = sol_calloc(1, sizeof(SolPatternStateGen));
	g->i = 0;
	return g;
}

void solPatternStateGen_free(SolPatternStateGen *g)
{
	sol_free(g);
}

SolPatternState* solPatternGen_gen_state(SolPatternStateGen *g)
{
	g->i++;
	g->l[g->i] = g->i;
	return &(g->l[g->i]);
}

int solPattern_is_match(SolPattern *p, SolPatternCharacter *s, size_t size)
{
	if (p == NULL) {
		return -3;
	}
	if (solPattern_dfa(p) == NULL) {
		return -4;
	}
	size_t i = 0;
	int r;
	SolPatternCharacter st[] = {'0'};
	solDfa_reset_current_state(solPattern_dfa(p));
	while (i < size) {
		solPattern_character_at_offset(st, s, i);
		r = solDfa_read_character(solPattern_dfa(p), st);
		if (r == 1) {
			return 2;
		} else if (r != 0) {
			return -1;
		}
		i++;
	}
	if (solDfa_is_accepting(solPattern_dfa(p))) {
		return 0;
	}
	return 1;
}

SolPattern* solPattern_empty_new(SolPatternStateGen *g)
{
	SolPattern *p = solPattern_new();
	SolPatternState *s = solPatternGen_gen_state(g);
	if (solDfa_set_starting_state(solPattern_dfa(p), s) != 0) {
		solPattern_free(p);
		return NULL;
	}
	if (solDfa_set_accepting_state(solPattern_dfa(p), s) != 0) {
		solPattern_free(p);
		return NULL;
	}
	return p;
}

SolPattern* solPattern_literal_new(SolPatternStateGen *g, SolPatternCharacter *c)
{
	SolPattern *p = solPattern_new();
	SolPatternState *s1 = solPatternGen_gen_state(g);
	SolPatternState *s2 = solPatternGen_gen_state(g);
	if (solDfa_set_starting_state(solPattern_dfa(p), s1) != 0) {
		solPattern_free(p);
		return NULL;
	}
	if (solDfa_set_accepting_state(solPattern_dfa(p), s2) != 0) {
		solPattern_free(p);
		return NULL;
	}
	solDfa_add_rule(solPattern_dfa(p), s1, s2, c);
	return p;
}

SolPattern* solPattern_repeat(SolPattern *p)
{
	if (p == NULL) {
		return NULL;
	}
	if (solDfa_state_merge(solPattern_dfa(p),
						   solPattern_dfa(p),
						   solDfa_starting_state(solPattern_dfa(p)),
						   solDfa_accepting_state(solPattern_dfa(p))
						   ) != 0
		) {
		return NULL;
	}
	if (solDfa_set_accepting_state(solPattern_dfa(p), solDfa_starting_state(solPattern_dfa(p))) != 0) {
		return NULL;
	}
	return p;
}

SolPattern* solPattern_concatenate(SolPattern *p1, SolPattern *p2)
{
	if (p1 == NULL || p2 == NULL) {
		return NULL;
	}
	void *sa2 = solDfa_accepting_state(solPattern_dfa(p2));
	if (solDfa_state_merge(solPattern_dfa(p1),
						   solPattern_dfa(p2),
						   solDfa_accepting_state(solPattern_dfa(p1)),
						   solDfa_starting_state(solPattern_dfa(p2))
						   ) != 0
		) {
		return NULL;
	}
	if (solDfa_set_accepting_state(solPattern_dfa(p1), sa2) != 0) {
		return NULL;
	}
	solPattern_free(p2);
	return p1;
}

SolPattern* solPattern_choose(SolPattern *p1, SolPattern *p2)
{
	if (p1 == NULL || p2 == NULL) {
		return NULL;
	}
	void *sa1 = solDfa_accepting_state(solPattern_dfa(p1));
	void *sa2 = solDfa_accepting_state(solPattern_dfa(p2));
	if (solDfa_state_merge(solPattern_dfa(p1),
						   solPattern_dfa(p2),
						   solDfa_starting_state(solPattern_dfa(p1)),
						   solDfa_starting_state(solPattern_dfa(p2))
						   ) != 0
		) {
		return NULL;
	}
	if (solDfa_state_merge(solPattern_dfa(p1), NULL, sa1, sa2) != 0) {
		return NULL;
	}
	solPattern_free(p2);
	return p1;
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

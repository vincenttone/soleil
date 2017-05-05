#include <stdio.h>
#include "sol_dfa.h"

SolDfaState* solDfaState_new(void *s)
{
	SolDfaState *ds = sol_calloc(1, sizeof(SolDfaState));
	if (ds == NULL) {
		return NULL;
	}
	solDfaState_set_state(ds, s);
	return ds;
}

void solDfaState_free(SolDfaState *ds)
{
	if (solDfaState_rules(ds)) {
		solHash_free(solDfaState_rules(ds));
	}
	sol_free(ds);
}

void _solDfaState_free(void *ds)
{
	solDfaState_free((SolDfaState*)ds);
}

int solDfaState_add_rule(SolDfaState *ds1, SolDfaState *ds2, void *c)
{
	if (solDfaState_rules(ds1)) {
		if (solHash_put(solDfaState_rules(ds1), c, ds2) == 0) {
			return 0;
		} else {
			return 1;
		}
	}
	return 2;
}

int solDfaState_remove_rule(SolDfaState *ds, void *c)
{
	if (solDfaState_rules(ds)) {
		solHash_del(solDfaState_rules(ds), c);
		return 0;
	}
	return 1;
}

SolDfaState* solDfaState_next(SolDfaState *ds, void *c)
{
	if (solDfaState_rules(ds)) {
		return solHash_get(solDfaState_rules(ds), c);
	}
	return NULL;
}

SolDfa* solDfa_new(sol_f_hash_ptr fsh1, sol_f_hash_ptr fsh2, sol_f_match_ptr fsm,
				   sol_f_hash_ptr fch1, sol_f_hash_ptr fch2, sol_f_match_ptr fcm)
{
	SolDfa *d = sol_calloc(1, sizeof(SolDfa));
	if (d == NULL) {
		return NULL;
	}
	solDfa_set_all_states(d, solHash_new());
	if (solDfa_all_states(d) == NULL) {
		solDfa_free(d);
		return NULL;
	}
	solDfa_set_state_hash_func1(d, fsh1);
	solDfa_set_state_hash_func2(d, fsh2);
	solDfa_set_state_match_func(d, fsm);
	solDfa_set_character_hash_func1(d, fch1);
	solDfa_set_character_hash_func2(d, fch2);
	solDfa_set_character_match_func(d, fcm);

	solHash_set_hash_func1(solDfa_all_states(d), solDfa_state_hash_func1(d));
	solHash_set_hash_func2(solDfa_all_states(d), solDfa_state_hash_func2(d));
	solHash_set_equal_func(solDfa_all_states(d), solDfa_state_match_func(d));
	solHash_set_free_v_func(solDfa_all_states(d), &_solDfaState_free);
	return d;
}

void solDfa_free(SolDfa *d)
{
	if (solDfa_all_states(d)) {
		solHash_free(solDfa_all_states(d));
	}
	sol_free(d);
}

int solDfa_set_starting_state(SolDfa *d, void *s)
{
	SolDfaState *ds = solHash_get(solDfa_all_states(d), s);
	if (ds == NULL) {
		ds = solDfaState_new(s);
		if (ds == NULL) {
			return 1;
		}
		solDfa_register_state(d, s, ds);
	}
	_solDfa_set_starting_state(d, s);
	solDfa_set_current_state(d, s);
	return 0;
}

int solDfa_set_accepting_state(SolDfa *d, void *s)
{
	SolDfaState *ds = solHash_get(solDfa_all_states(d), s);
	if (ds == NULL) {
		ds = solDfaState_new(s);
		if (ds == NULL) {
			return 1;
		}
		solDfa_register_state(d, s, ds);
	}
	_solDfa_set_accepting_state(d, s);
	return 0;
}

void solDfa_remove_dfa_state(SolDfa *d, void *s)
{
	if (solDfa_all_states(d)) {
		solHash_remove_by_key(solDfa_all_states(d), s);
	}
}

int solDfa_init_dfa_state_rule(SolDfa *d, SolDfaState *ds)
{
	solDfaState_set_rules(ds, solHash_new());
	if (solDfaState_rules(ds) == NULL) {
		return -1;
	}
	solHash_set_hash_func1(solDfaState_rules(ds), solDfa_character_hash_func1(d));
	solHash_set_hash_func2(solDfaState_rules(ds), solDfa_character_hash_func2(d));
	solHash_set_equal_func(solDfaState_rules(ds), solDfa_character_match_func(d));
	return 0;
}

int solDfa_add_rule(SolDfa *d, void *s1, void *s2, void *c)
{
	SolDfaState *ds1 = solHash_get(solDfa_all_states(d), s1);
	if (ds1 == NULL) {
		ds1 = solDfaState_new(s1);
		if (ds1 == NULL) {
			return -1;
		}
		solDfa_register_state(d, s1, ds1);
	}
	if (solDfaState_rules(ds1) == NULL) {
		if (solDfa_init_dfa_state_rule(d, ds1) != 0) {
			return -2;
		}
	}
	SolDfaState *ds2 = solHash_get(solDfa_all_states(d), s2);
	if (ds2 == NULL) {
		ds2 = solDfaState_new(s2);
		if (ds2 == NULL) {
			return -1;
		}
		solDfa_register_state(d, s2, ds2);
	}
	if (solDfaState_add_rule(ds1, ds2, c) == 0) {
		return 0;
	}
	return -3;
}

int solDfa_read_character(SolDfa *d, void *c)
{
	if (solDfa_current_state(d) == NULL) {
		return -1;
	}
	SolDfaState *ds = solDfa_conv_dfa_state(d, solDfa_current_state(d));
	if (ds == NULL) {
		return -2;
	}
	ds = solDfaState_next(ds, c);
	if (ds == NULL) {
		solDfa_set_current_state(d, NULL);
		return 1;
	}
	solDfa_set_current_state(d, solDfaState_state(ds));
	return 0;
}

int solDfa_state_merge(SolDfa *d1, SolDfa *d2, void *s1, void *s2)
{
	if (d1 == NULL || s1 == NULL || s2 == NULL) {
		return -1;
	}
	if (solDfa_state_match(d1, s1, s2) == 0) {
		return 0;
	}
	if (d2 == NULL) {
		d2 = d1;
	}
	SolDfaState *ds1 = solHash_get(solDfa_all_states(d1), s1);
	SolDfaState *ds2 = solHash_get(solDfa_all_states(d2), s2);
	if (ds1 == NULL || ds2 == NULL) {
		return -2;
	}
	if (ds1 != ds2) {
		solHash_merge(solDfa_all_states(d1), solDfa_all_states(d2));
	}
	SolHashIter *i;
	SolDfaState *dsn;
	SolDfaState *ds2n;
	SolHashRecord *r = NULL;
	SolHashRecord *r2 = NULL;
	// lookup forward
	if (solDfaState_rules(ds2)) {
		if (solDfaState_rules(ds1) == NULL
			&& solDfa_init_dfa_state_rule(d1, ds1) != 0) {
			return -3;
		}
		void *c2;
		i = solHashIter_new(solDfaState_rules(ds2));
		solHashIter_rewind(i);
		while ((r2 = solHashIter_get(i))) {
			c2 = r2->k;
			ds2n = (SolDfaState*)(r2->v);
			r = solHash_get(solDfaState_rules(ds1), c2);
			if (r) {
				dsn = (SolDfaState*)(r->v);
				if (solDfaState_state(ds1) != solDfaState_state(dsn)
					&& solDfaState_state(ds2) != solDfaState_state(ds2n)) {
					solDfa_state_merge(d1, NULL, solDfaState_state(dsn), solDfaState_state(ds2n));
				}
			} else {
				solDfaState_add_rule(ds1, ds2n, c2);
			}
		}
		solHashIter_free(i);
	}
	// lookup backward
	i = solHashIter_new(solDfa_all_states(d2));
	solHashIter_rewind(i);
	SolHashIter *in;
	while ((r2 = solHashIter_get(i))) {
		ds2n = (SolDfaState*)(r2->v);
		if (solDfaState_rules(ds2n)) {
			in = solHashIter_new(solDfaState_rules(ds2n));
			solHashIter_rewind(in);
			while ((r = solHashIter_get(in))) {
				dsn = (SolDfaState*)(r->v);
				// redirect ds2's parent relations to ds1
				if (solDfa_dfa_state_match(d2, ds2, dsn) == 0) {
					solDfaState_add_rule(ds2n, ds1, r->k);
				}
			}
			solHashIter_free(in);
		}
	}
	solHashIter_free(i);
	if (d1 != d2) {
		solHash_wipe(solDfa_all_states(d2));
	}
	if (solDfa_state_match(d1, s1, s2) != 0) {
		solDfa_remove_dfa_state(d1, s2);
		solDfaState_free(ds2);
	}
	return 0;
}

void _solDfa_debug_relations(SolDfa *d)
{
	printf("Starting state: (%d):\n", *(int*)solDfa_starting_state(d));
	printf("Accepting state: (%d):\n", *(int*)solDfa_accepting_state(d));
	printf("All states:\n");
	int *n;
	SolHashIter *i = solHashIter_new(solDfa_all_states(d));
	SolHashIter *j;
	SolDfaState *s;
	SolHashRecord *r;
	solHashIter_rewind(i);
	while ((r = solHashIter_get(i))) {
		n = (int*)r->k;
		s = r->v;
		if (solDfaState_rules(s)) {
			j = solHashIter_new(solDfaState_rules(s));
			solHashIter_rewind(j);
			while ((r = solHashIter_get(j))) {
				printf("rules: (%d) -(%c)-> (%d)\n", *n, *((char*)r->k), *(int*)(((SolDfaState*)r->v)->s));
			}
			solHashIter_free(j);
		}
	}
	solHashIter_free(i);
}

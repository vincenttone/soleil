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

void solDfa_remove_dfa_state(SolDfa *d, void *s)
{
	if (solDfa_all_states(d)) {
		solHash_remove_by_key(solDfa_all_states(d), s);
	}
}

int solDfa_add_rule(SolDfa *d, void *s1, void *s2, void *c)
{
	SolDfaState *ds1 = solHash_get(solDfa_all_states(d), s1);
	if (ds1 == NULL) {
		ds1 = solDfaState_new(s1);
		if (ds1 == NULL) {
			return 1;
		}
	}
	solDfa_register_state(d, s1, ds1);
	if (solDfaState_rules(ds1) == NULL) {
		solDfaState_set_rules(ds1, solHash_new());
		if (solDfaState_rules(ds1) == NULL) {
			return 2;
		}
		solHash_set_hash_func1(solDfaState_rules(ds1), solDfa_character_hash_func1(d));
		solHash_set_hash_func2(solDfaState_rules(ds1), solDfa_character_hash_func2(d));
		solHash_set_equal_func(solDfaState_rules(ds1), solDfa_character_match_func(d));
	}
	SolDfaState *ds2 = solHash_get(solDfa_all_states(d), s2);
	if (ds2 == NULL) {
		ds2 = solDfaState_new(s2);
		if (ds2 == NULL) {
			return 1;
		}
	}
	solDfa_register_state(d, s2, ds2);
	if (solDfaState_add_rule(ds1, ds2, c) == 0) {
		return 0;
	}
	return 3;
}

int solDfa_read_character(SolDfa *d, void *c)
{
	if (solDfa_current_state(d) == NULL) {
		return 1;
	}
	SolDfaState *ds = solDfa_conv_dfa_state(d, solDfa_current_state(d));
	if (ds == NULL) {
		return 2;
	}
	ds = solDfaState_next(ds, c);
	if (ds == NULL) {
		solDfa_set_current_state(d, NULL);
		return 3;
	}
	solDfa_set_current_state(d, solDfaState_state(ds));
	return 0;
}

int solDfaState_merge(SolDfa *d, SolDfaState *ds1, SolDfaState *ds2, SolHash *map)
{
	SolHashIter *i1 = solHashIter_new(solDfaState_rules(ds1));
	SolHashIter *i2 = solHashIter_new(solDfaState_rules(ds2));
	void *c;
	SolDfaState *nds;
	SolHashRecord *r;
	solHashIter_rewind(i1);
	while ((r = solHashIter_get(i1))) {
		c = r->k;
		nds = (SolDfaState*)(r->v);
		solHashIter_rewind(i2);
		while ((r = solHashIter_get(i2))) {
			// merge 1,3
			// 1--a-->2
			// 3--a-->4
			// 3--b-->5 | 3--c-->6
			// result: 1--a-->2 | 1--b-->5 | 1--c-->6, merge 2,4
			if (solDfa_character_match(d, c, r->k)) {
				solDfaState_merge(d, nds, (SolDfaState*)(r->v), map);
			} else {
				solDfaState_add_rule(ds1, (SolDfaState*)(r->v), (void*)(r->v));
			}
		}
	}
	solHash_put(map, ds2->s, ds1);
	solDfa_remove_dfa_state(d, ds2->s);
	solDfaState_free(ds2);
	return 0;
}

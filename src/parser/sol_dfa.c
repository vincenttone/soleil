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
	solDfa_set_accepting_states(d, solSet_new());
	if (solDfa_all_states(d) == NULL || solDfa_accepting_states(d) == NULL) {
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

	solSet_set_hash_func1(solDfa_accepting_states(d), solDfa_state_hash_func1(d));
	solSet_set_hash_func2(solDfa_accepting_states(d), solDfa_state_hash_func2(d));
	solSet_set_equal_func(solDfa_accepting_states(d), solDfa_state_match_func(d));
	return d;
}

void solDfa_free(SolDfa *d)
{
	if (solDfa_all_states(d)) {
		solDfa_free_all_states(d);
	}
	if (solDfa_accepting_states(d)) {
		solSet_free(solDfa_accepting_states(d));
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

int solDfa_add_accepting_state(SolDfa *d, void *s)
{
	SolDfaState *ds = solHash_get(solDfa_all_states(d), s);
	if (ds == NULL) {
		ds = solDfaState_new(s);
		if (ds == NULL) {
			return 1;
		}
		solDfa_register_state(d, s, ds);
	}
	solSet_add(solDfa_accepting_states(d), s);
	return 0;
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
	SolHashIter *i;
	SolDfaState *dsn;
	SolDfaState *ds2n;
	SolHashRecord *r = NULL;
	SolHashRecord *r2 = NULL;
	void *c;
	// downstream
	if (solDfaState_rules(ds2)) {
		if (solDfaState_rules(ds1) == NULL
			&& solDfa_init_dfa_state_rule(d1, ds1) != 0) {
			return -3;
		}
		i = solHashIter_new(solDfaState_rules(ds2));
		solHashIter_rewind(i);
		while ((r2 = solHashIter_get(i))) {
			c = r2->k;
			ds2n = (SolDfaState*)(r2->v);
			dsn = solDfaState_next(ds1, c);
			if (dsn) {
				if (solDfa_dfa_state_match(d1, ds1, dsn) != 0
					|| solDfa_dfa_state_match(d2, ds2, ds2n) != 0
					) {
					solDfa_state_merge(d1, NULL, solDfaState_state(dsn), solDfaState_state(ds2n));
				}
			} else {
				solDfaState_add_rule(ds1, ds2n, c);
			}
		}
		solHashIter_free(i);
	}
	// upstream
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
				// dsn --c--> ds2
				if (solDfa_dfa_state_match(d2, ds2, dsn) == 0) {
					solDfaState_add_rule(ds2n, ds1, r->k);
				}
			}
			solHashIter_free(in);
		}
	}
	solHashIter_free(i);
	if (solDfa_state_match(d1, s1, s2) != 0) {
		if (solDfa_state_in_accepting_states(d2, s2) == 0) {
			solDfa_add_accepting_state(d2, s1);
		}
		if (solDfa_state_is_starting_state(d2, s2)) {
			solDfa_set_starting_state(d2, s1);
		}
		solHash_remove(solDfa_all_states(d2), s2);
	}
	return 0;
}

int solDfa_is_accepting(SolDfa *dfa)
{
	if (solDfa_accepting_states(dfa) == NULL) {
		return -1;
	}
	if (solDfa_current_state(dfa) == NULL) {
		return 1;
	}
	if (solDfa_state_in_accepting_states(dfa, solDfa_current_state(dfa)) == 0) {
		return 0;
	}
	return 2;
}

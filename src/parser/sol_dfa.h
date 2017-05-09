#ifndef _SOL_DFA_H_
#define _SOL_DFA_H_ 1

#include "sol_common.h"
#include "sol_hash.h"
#include "sol_set.h"

typedef struct _SolDfaState {
	void *s; // state
	SolHash *r; // rules {character: dfa_state, ...}
} SolDfaState;

typedef struct _SolDfa {
	void *ss; //starting state
	SolSet *as; // accepting states
	void *cs; // current state
	SolHash *als; // all states
	sol_f_hash_ptr f_s_hash1; // state hash func1
	sol_f_hash_ptr f_s_hash2; // state hash func2
	sol_f_hash_ptr f_c_hash1; // character hash func1
	sol_f_hash_ptr f_c_hash2; // character hash func2
	sol_f_match_ptr f_sm; // func state match
	sol_f_match_ptr f_cm; // func character match
} SolDfa;

#define solDfaState_set_state(ds, s) (ds)->s = s
#define solDfaState_set_rules(ds, h) (ds)->r = h
#define solDfaState_state(ds) (ds)->s
#define solDfaState_rules(ds) (ds)->r

#define _solDfa_set_starting_state(d, s) (d)->ss = s; 
#define solDfa_set_current_state(d, s) (d)->cs = s
#define solDfa_set_accepting_states(d, ss) (d)->as = ss
#define solDfa_set_all_states(d, ds) (d)->als = ds
#define solDfa_reset_current_state(d) solDfa_set_current_state(d, d->ss)
#define solDfa_register_state(d, s, ds) solHash_put(solDfa_all_states(d), s, ds)
#define solDfa_conv_dfa_state(d, s) solHash_get(solDfa_all_states(d), s)

#define solDfa_starting_state(d) (d)->ss
#define solDfa_current_state(d) (d)->cs
#define solDfa_accepting_states(d) (d)->as
#define solDfa_all_states(d) (d)->als

#define solDfa_free_all_states(d) solHash_free(solDfa_all_states(d))
#define solDfa_wipe_all_states(d) solHash_wipe(solDfa_all_states(d))
#define solDfa_merge_all_states(d1, d2) solHash_merge(solDfa_all_states(d1), \
													  solDfa_all_states(d2))

#define solDfa_state_in_accepting_states(d, s) solSet_in_set(solDfa_accepting_states(d), s)
#define solDfa_accepting_states_rewind(d) solSet_rewind(solDfa_accepting_states(d))
#define solDfa_accepting_states_get_one(d) solSet_get(solDfa_accepting_states(d))
#define solDfa_wipe_accepting_states(d) solSet_wipe(solDfa_accepting_states(d))
#define solDfa_merge_accepting_states(d1, d2) solSet_merge(solDfa_accepting_states(d1), \
														   solDfa_accepting_states(d2))


#define solDfa_set_character_hash_func1(d, f) d->f_c_hash1 = f
#define solDfa_set_character_hash_func2(d, f) d->f_c_hash2 = f
#define solDfa_set_character_match_func(d, f) d->f_cm = f
#define solDfa_set_state_hash_func1(d, f) d->f_s_hash1 = f
#define solDfa_set_state_hash_func2(d, f) d->f_s_hash2 =f 
#define solDfa_set_state_match_func(d, f) d->f_sm = f

#define solDfa_character_hash_func1(d) d->f_c_hash1
#define solDfa_character_hash_func2(d) d->f_c_hash2
#define solDfa_character_match_func(d) d->f_cm
#define solDfa_state_hash_func1(p) p->f_s_hash1
#define solDfa_state_hash_func2(p) p->f_s_hash2
#define solDfa_state_match_func(d) d->f_sm

#define solDfa_character_match(d, c1, c2) (*d->f_cm)(c1, c2)
#define solDfa_state_match(d, s1, s2) (*d->f_sm)(s1, s2)
#define solDfa_dfa_state_match(d, ds1, ds2) solDfa_state_match(d,		\
															   solDfaState_state(ds1), \
															   solDfaState_state(ds2))
#define solDfa_state_is_starting_state(d, s) (solDfa_state_match(d, solDfa_starting_state(d), s) == 0)

SolDfaState* solDfaState_new(void*);
void solDfaState_free(SolDfaState*);
void _solDfaState_free(void*);
int solDfaState_add_rule(SolDfaState*, SolDfaState*, void*);
SolDfaState* solDfaState_next(SolDfaState*, void*);

SolDfa* solDfa_new(sol_f_hash_ptr, sol_f_hash_ptr, sol_f_match_ptr,
				   sol_f_hash_ptr, sol_f_hash_ptr, sol_f_match_ptr);
void solDfa_free(SolDfa*);
int solDfa_set_starting_state(SolDfa*, void*);
int solDfa_add_accepting_state(SolDfa*, void*);
int solDfa_is_accepting(SolDfa*);
int solDfa_add_rule(SolDfa*, void*, void*, void*);
int solDfa_read_character(SolDfa*, void*);

int solDfa_init_dfa_state_rule(SolDfa*, SolDfaState*);

int solDfa_state_merge(SolDfa*, SolDfa*, void*, void*);

void _solDfa_debug_relations(SolDfa *d);

#endif

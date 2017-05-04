#include <string.h>
#include <stdio.h>
#include "sol_utils.h"
#include "sol_dfa.h"

int _state_equal(void *s1, void *s2)
{
	if ((int*)s1 == (int*)s2) {
		return 0;
	}
	return 1;
}

int _character_equal(void *s1, void *s2)
{
	if ((char*)s1 == (char*)s2) {
		return 0;
	}
	return 1;
}

void print_current_state(SolDfa *d)
{
	if (solDfa_current_state(d)) {
		printf("current state is %d\n", *(int*)(solDfa_current_state(d)));
	} else {
		printf("current state empty\n");
	}
	if (solDfa_is_accepting(d)) {
		printf("ACCEPTING\n");
	}
}

int main()
{
	int i = 1;
	int ii = 2;
	int iii = 3;
	int iv = 4;
	char a = 'a';
	char b = 'b';
	char c = 'c';
	SolDfa *d = solDfa_new(&sol_i_hash_func1, &sol_i_hash_func2, &_state_equal,
						   &sol_c_hash_func1, &sol_c_hash_func2, &_character_equal);
	solDfa_set_starting_state(d, &i);
	solDfa_set_accepting_state(d, &iv);
	if (solDfa_add_rule(d, &i, &ii, &a) != 0) {
		goto err;
	}
	_solDfa_debug_relations(d);
	if (solDfa_add_rule(d, &i, &iii, &b) != 0) {
		goto err;
	}
	_solDfa_debug_relations(d);
	if (solDfa_add_rule(d, &ii, &iv, &c) != 0) {
		goto err;
	}
	if (solDfa_add_rule(d, &iii, &iv, &c) != 0) {
		goto err;
	}
	solDfa_read_character(d, &a);
	print_current_state(d);
	solDfa_read_character(d, &a);
	print_current_state(d);
	solDfa_read_character(d, &b);
	print_current_state(d);
	solDfa_reset_current_state(d);
	print_current_state(d);
	solDfa_read_character(d, &a);
	print_current_state(d);
	solDfa_read_character(d, &c);
	print_current_state(d);
	solDfa_free(d);
	return 0;
 err:
	solDfa_free(d);
	return 1;
}

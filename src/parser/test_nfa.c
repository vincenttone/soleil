#include <stdio.h>
#include <string.h>
#include "sol_nfa.h"

void print_current_states(SolNfa*);

void print_current_states(SolNfa *nfa)
{
	SolNfaStates *s = nfa->current_states;
	void *c;
	solSet_rewind(s);
	while ((c = solSet_get(s))) {
		printf("Got:\t%s\n", (char*)c);
		printf("Set size: %d\t, iter num: %d\n", (int)solSet_size(s), (int)solSetIter_num(s));
	}
}

int main()
{
	SolNfa *nfa = solNfa_new();
	solNfa_add_rule(nfa, 1, 1, 'a');
	solNfa_add_rule(nfa, 1, 2, 'b');
	solNfa_add_rule(nfa, 2, 1, 'b');
	solNfa_add_rule(nfa, 1, 3, 'b');
	solNfa_add_rule(nfa, 2, 4, 'c');
	solNfa_add_rule(nfa, 3, 4, 'd');
	solNfa_add_rule(nfa, 4, 5, 'd');
	solNfa_add_current_state(nfa, (char*)"1");
	solNfa_add_accept_state(nfa, (char*)"4");
	solNfa_add_accept_state(nfa, (char*)"5");
	SOL_FA_STATE *s = NULL;
	int r;
	solNfa_read_character(nfa, (char*)"b");
	r = solNfa_accepting(nfa, s);
	printf("accepting? %d\n", r);
	print_current_states(nfa);
	solNfa_read_character(nfa, (char*)"a");
	r = solNfa_accepting(nfa, s);
	printf("accepting? %d\n", r);
	solNfa_read_character(nfa, (char*)"c");
	r = solNfa_accepting(nfa, s);
	printf("accepting? %d\n", r);
	
	solNfa_free(nfa);
	return 0;
}

#include "sol_nfa.h"

SolNfa* solNfa_new()
{
	SolNfa *nfa = sol_alloc(sizeof(SolNfa));
	nfa->book = sol_alloc(sizeof(SolNfaRuleBook));
	nfa->current_states = NULL;
	nfa->accept_states = NULL;
}

void solNfa_free(SolNfa *nfa)
{
	if (nfa->current_states) {
		solSet_free(nfa->current_states);
	}
	if (nfa->accept_states) {
		solSet_free(nfa->accept_states);
	}
	if (nfa->book) {
		solNfaRuleBook_free(nfa->book);
	}
	sol_free(nfa);
}

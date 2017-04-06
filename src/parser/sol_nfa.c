#include <stdbool.h>
#include "sol_nfa.h"

SolNfa* solNfa_new()
{
	SolNfa *nfa = sol_alloc(sizeof(SolNfa));
	nfa->book = solNfaRuleBook_new();
	nfa->current_states = solNfaStates_new();
	nfa->accept_states = solNfaStates_new();
	return nfa;
}

void solNfa_free(SolNfa *nfa)
{
	solNfaStates_free(nfa->current_states);
	solNfaStates_free(nfa->accept_states);
	solNfaRuleBook_free(nfa->book);
	sol_free(nfa);
}

int solNfa_accepting(SolNfa *nfa, SOL_FA_STATE_PTR s)
{
	SolNfaStates *cs = solNfa_current_states(nfa);
	solNfaStates_rewind(cs);
	while (cs && (s = solNfaStates_get(cs))) {
		if (solNfaStates_in(nfa->accept_states, s)) {
			return 0;
		}
	}
	return 1;
}

void solNfa_read_character(SolNfa *nfa, SOL_FA_CHARACTER *c)
{
	SolNfaStates *s = solNfaStates_new();
	solNfaRuleBook_next_states(s, nfa->book, solNfa_current_states(nfa), c);
	solNfaStates_free(s);
}

SolNfaStates* solNfa_current_states(SolNfa *nfa)
{
	solNfa_free_moves(nfa);
	return nfa->current_states;
}

SolNfa* solNfa_free_moves(SolNfa *nfa)
{
	SolNfaStates *s = solNfaStates_new();
	if (solNfaRuleBook_free_moves(nfa->current_states, nfa->book, s)) {
		solNfaStates_free(s);
		return nfa;
	}
	solNfaStates_free(s);
	return NULL;
}

SolNfaRuleBook* solNfaRuleBook_new()
{
	SolNfaRuleBook *book = sol_alloc(sizeof(SolNfaRuleBook));
	book->rules = solList_new();
	book->rules->free = (void*)sol_destory_fa_rule;
	book->iter = solListIter_new(book->rules);
	return book;
}

void solNfaRuleBook_free(SolNfaRuleBook *book)
{
	solListIter_free(book->iter);
	solList_free(book->rules);
	sol_free(book);
}

SolNfaRuleBook* solNfaRuleBook_add_rule(SolNfaRuleBook *book, SOL_FA_STATE s1, SOL_FA_STATE s2, SOL_FA_CHARACTER c)
{
	SolFaRule *r = sol_new_fa_rule(s1, s2, c);
	if (r == NULL) {
		return NULL;
	}
	solList_add_node(book->rules, r);
	return book;
}

void solNfaRuleBookIter_rewind(SolNfaRuleBook *book)
{
	solListIter_rewind(book->rules, book->iter);
}

SOL_FA_STATE solNfaRuleBookIter_follow_rule(SolNfaRuleBook *book, SOL_FA_STATE_PTR s, SOL_FA_CHARACTER_PTR c)
{
	SolFaRule *rule;
	SolListNode *node;
	node = solListIter_next(book->iter);
	if (node) {
		rule = node->val;
		if (sol_fa_rule_applies_to(rule, *s, *c)) {
			return sol_fa_rule_next_state(rule);
		}
		return solNfaRuleBookIter_follow_rule(book, s, c);
	}
	return SOL_FA_STATE_NONE;
}

SolNfaStates* solNfaRuleBook_next_states(SolNfaStates *ns, SolNfaRuleBook *book, SolNfaStates *cs, SOL_FA_CHARACTER_PTR c)
{
	SOL_FA_STATE_PTR s;
	SOL_FA_STATE ts;
	solNfaRuleBookIter_rewind(book);
	solNfaStates_rewind(cs);
	while ((s = solNfaStates_get(cs))) {
		ts = solNfaRuleBookIter_follow_rule(book, s, c);
		if (ts == SOL_FA_STATE_NONE) {
			break;
		}
		solNfaStates_add(ns, (SOL_FA_STATE_PTR)ts);
	}
	if (solNfaStates_not_empty(ns)) {
		return ns;
	}
	return NULL;
}

SolNfaStates* solNfaRuleBook_free_moves(SolNfaStates *cs, SolNfaRuleBook *book, SolNfaStates *s)
{
	if (solNfaRuleBook_next_states(s, book, cs, SOL_FA_STATE_NONE)) {
		if (solNfaStates_is_substates(cs, s) == 1) {
			if (solNfaStates_merge(cs, s) == 0) {
				solNfaRuleBook_free_moves(cs, book, s);
			} else {
				return NULL;
			}
		}
	}
	return cs;
}

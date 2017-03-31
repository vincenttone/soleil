#include <stdbool.h>
#include "sol_nfa.h"

SolNfa* solNfa_new(void (*s_new), void (*s_free))
{
	SolNfa *nfa = sol_alloc(sizeof(SolNfa));
	nfa->book = solNfaBook_new();
	if (s_new) {
		nfa->s_new = s_new;
		nfa->current_states = nfa->s_new();
		nfa->accept_states = nfa->s_new();
	}
	if (s_free) {
		nfa->s_free = s_free;
	}
}

void solNfa_free(SolNfa *nfa)
{
	if (nfa->s_free && nfa->current_states) {
		nfa->s_free(nfa->current_states);
	}
	if (nfa->accept_states) {
		nfa->s_free(nfa->accept_states);
	}
	if (nfa->book) {
		solNfaRuleBook_free(nfa->book);
	}
	sol_free(nfa);
}

int solNfa_accepting(SolNfa *nfa)
{
	void *cs = solNfa_current_states(nfa);
	SOL_FA_STATE *s;
	nfa->s_rewind(cs);
	while (cs && (s = nfa->s_get(cs))) {
		if (nfa->s_in(nfa->accept_states, s)) {
			return 0;
		}
	}
	return 1;
}

void solNfa_read_character(SolNfa *nfa, SOL_FA_CHARACTER c)
{
	void *s = nfa->s_new();
	solNfaRuleBook_next_states(s, nfa->book, solNfa_current_states(nfa), c);
	nfa->s_free(s);
}

void* solNfa_current_states(SolNfa *nfa)
{
	solNfa_free_moves(nfa);
	return nfa->current_states;
}

SolNfa* solNfa_free_moves(SolNfa *nfa)
{
	SolSet *s = nfa->s_new();
	if (solNfaRuleBook_free_moves(nfa->current_states, nfa->book, s)) {
		nfa->s_free(s);
		return nfa;
	}
	nfa->s_free(s);
	return NULL;
}

solNfaBook* solNfaBook_new()
{
	solNfaBook *book = sol_alloc(sizeof(solNfaBook));
	book->rules = solList_new();
	book->rules->free = (void*)sol_destory_fa_rule;
	book->iter = solListIterator_new(book->rules);
	return book;
}

void solNfaRuleBook_free(SolNfaRuleBook *book)
{
	solList_release_iterator(book->iter);
	solList_release(book->rules);
	sol_free(book);
}

SolNfaRuleBook* solNfaRuleBook_add_rule(SolNfaRuleBook *book, SOL_FA_STATE s1, SOL_FA_STATE s2, SOL_FA_CHARACTER c)
{
	solFaRule *r = sol_new_fa_rule(s1, s2, c);
	if (r == NULL) {
		return r;
	}
	solList_add_node(book->rules, r);
	return book;
}

void solNfaRuleBookIter_rewind(solNfaRuleBook *book)
{
	solListIter_rewind(book->rules, book->iter);
}

SOL_FA_STATE solNfaRuleBookIter_follow_rule(SOL_FA_STATE s1, SOL_FA_CHARACTER c)
{
	SolFaRule *rule;
	SolListNode *node;
	node = solListIter_next(book->iter);
	if (node) {
		rule = node->val;
		if (sol_fa_rule_applies_to(rule, s1, c) == true) {
			return sol_fa_rule_next_state(rule);
		}
		return solNfaRuleBookIter_follow_rule(SolSet *s, SOL_FA_STATE s1, SOL_FA_CHARACTER c);
	}
	return SOL_FA_STATE_NONE;
}

void* solNfaRuleBook_next_states(solSet *ns, SolNfaRuleBook *book, SolSet *cs, SOL_FA_CHARACTER c)
{
	SOL_FA_STATE s;
	solNfaRuleBookIter_rewind(book);
	solSet_rewind(cs);
	while ((s = solSet_get(cs))) {
		s = solNfaRuleBookIter_follow_rules(s, c);
		if (s == SOL_FA_STATE_NONE) {
			break;
		}
		ns = solSet_add(s);
	}
	if (solSet_is_not_empty(ns)) {
		return ns;
	}
	return NULL;
}

SolSet* solNfaRuleBook_free_moves(SolSet *cs, SolNfaRuleBook *book, SolSet *s)
{
	if (solNfaRuleBook_next_states(s, book, cs, SolNil)) {
		if (solSet_is_subset(cs, s) == 1) {
			if (solSet_merge(cs, s) == 0) {
				solNfaRuleBook_free_moves(cs, book, s);
			} else {
				return NULL;
			}
		}
	}
	return book->current_states;
}

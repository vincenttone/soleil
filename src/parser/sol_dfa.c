#include <stdlib.h>
#include <stdio.h>
#include "sol_dfa.h"

SolDfaRuleBook* solDfaRuleBook_new()
{
	SolDfaRuleBook *book;
	book = sol_alloc(sizeof(SolDfaRuleBook));
	SolList *rules;
	rules = sol_alloc(sizeof(SolList));
	sol_list_init(rules);
	rules->free = (void*)solFaRule_free;
	book->rules = rules;
	SolListIter *iter = sol_list_get_iterator(rules);
	book->iter = iter;
	return book;
}

void solDfaRuleBook_free(SolDfaRuleBook *book)
{
	if (book == NULL) {
		return;
	}
	sol_list_release(book->rules);
	sol_list_release_iterator(book->iter);
	sol_free(book);
}

SOL_FA_STATE solDfaRuleBook_next_state(SolDfaRuleBook *book, SOL_FA_STATE state, SOL_FA_CHARACTER character)
{
	SolFaRule *rule;
	SolListNode *node;
	sol_list_rewind(book->rules, book->iter);
	for (;;) {
		node = sol_list_next_node(book->iter);
		if (node == NULL) {
			break;
		}
		rule = node->val;
		if (solFaRule_applies_to(rule, state, character)) {
			return solFaRule_next_state(rule);
		}
	}
	return SOL_FA_STATE_NONE;
}

SolFaRule* solDfaRuleBook_rule_for(SolDfaRuleBook *book, SOL_FA_STATE state, SOL_FA_CHARACTER character)
{
	SolFaRule *rule;
	SolListNode *node;
	sol_list_rewind(book->rules, book->iter);
	for (;;) {
		node = sol_list_next_node(book->iter);
		if (node == NULL) {
			break;
		}
		rule = node->val;
		if (solFaRule_applies_to(rule, state, character) == true) {
			return rule;
		}
	}
	return NULL;
}

SolDfa* solDfa_new(SolDfaRuleBook *rule_book, SOL_FA_STATE current_state, SOL_FA_STATE accept_state)
{
	SolDfa *dfa;
	dfa = sol_alloc(sizeof(SolDfa));
	if (dfa == NULL) {
		return NULL;
	}
	dfa->rule_book = rule_book;
	dfa->current_state = current_state;
	dfa->accept_state = accept_state;
	return dfa;
}

void solDfa_free(SolDfa *dfa)
{
	solDfaRuleBook_free(dfa->rule_book);
	sol_free(dfa);
}

bool solDfa_is_accepting(SolDfa *dfa)
{
	if (dfa->current_state == dfa->accept_state) {
		return true;
	}
	return false;
}

void solDfa_read_character(SolDfa *dfa, SOL_FA_CHARACTER character)
{
	if (dfa == NULL || dfa->rule_book == NULL) {
		return;
	}
	SOL_FA_STATE s = solDfaRuleBook_next_state(dfa->rule_book, dfa->current_state, character);
	if (s != SOL_FA_STATE_NONE) {
		dfa->current_state = s;
	}
}

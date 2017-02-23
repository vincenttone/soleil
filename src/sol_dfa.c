#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sol_dfa.h"

SolFaRule* sol_new_fa_rule(SOL_FA_STATE state, SOL_FA_STATE next_state, SOL_FA_CHARACTER character)
{
	SolFaRule *rule;
	rule = sol_alloc(sizeof(SolFaRule));
	if (rule == NULL) {
		return NULL;
	}
	rule->state = state;
	rule->next_state = next_state;
	rule->character = character;
	return rule;
}

void sol_destory_fa_rule(SolFaRule *rule)
{
	sol_free(rule);
}

bool sol_fa_rule_applies_to(SolFaRule *rule, SOL_FA_STATE state, SOL_FA_CHARACTER character)
{
	if (rule->state == state && rule->character == character) {
		return true;
	}
	return false;
}

SOL_FA_STATE sol_fa_rule_current_state(SolFaRule *rule)
{
	return rule->state;
}

SOL_FA_STATE sol_fa_rule_next_state(SolFaRule *rule)
{
	return rule->next_state;
}

SOL_FA_CHARACTER sol_fa_rule_character(SolFaRule *rule)
{
	return rule->character;
}

char* sol_fa_rule_inspect(SolFaRule *rule)
{
	char* i = sol_alloc(sizeof(char) * SOL_FA_RULE_INS_LEN);
	sprintf(i, "--FA: state <%ld> -> <%ld>, character: <%c>--\n",
			rule->state, rule->next_state, rule->character);
	return i;
}

SolDfaRuleBook* sol_dfa_rule_book_new()
{
	SolDfaRuleBook *book;
	book = sol_alloc(sizeof(SolDfaRuleBook));
	SolList *rules;
	rules = sol_alloc(sizeof(SolList));
	sol_list_init(rules);
	book->rules = rules;
	SolListIter *iter = sol_list_get_iterator(rules);
	book->iter = iter;
	return book;
}

void sol_dfa_rule_book_release(SolDfaRuleBook *book)
{
	if (book == NULL) {
		return;
	}
	SolFaRule *rule;
	SolListNode *node;
	sol_list_rewind(book->rules, book->iter);
	for (;;) {
		node = sol_list_next_node(book->iter);
		if (node == NULL) {
			break;
		}
		rule = node->val;
		sol_destory_fa_rule(rule);
	}
	sol_list_release(book->rules);
	sol_list_release_iterator(book->iter);
	sol_free(book);
}

SOL_FA_STATE sol_dfa_rule_book_next_state(SolDfaRuleBook *book, SOL_FA_STATE state, SOL_FA_CHARACTER character)
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
		if (sol_fa_rule_applies_to(rule, state, character)) {
			return sol_fa_rule_next_state(rule);
		}
	}
	return SOL_FA_STATE_NONE;
}

SolFaRule* sol_dfa_rule_book_rule_for(SolDfaRuleBook *book, SOL_FA_STATE state, SOL_FA_CHARACTER character)
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
		if (sol_fa_rule_applies_to(rule, state, character) == true) {
			return rule;
		}
	}
	return NULL;
}

char *sol_dfa_rule_book_inspect(SolDfaRuleBook *book)
{
	SolFaRule *rule;
	SolListNode *node;
	unsigned count = sol_dfa_rule_book_len(book);
	unsigned len = SOL_FA_RULE_INS_LEN * count;
	char *i = sol_alloc(sizeof(char) * len + 13);
	strcat(i, "DFA rulebook:\n");
	char *ir = sol_alloc(sizeof(char) * SOL_FA_RULE_INS_LEN);
	sol_list_rewind(book->rules, book->iter);
	for (;;) {
		node = sol_list_next_node(book->iter);
		if (node == NULL) {
			break;
		}
		rule = node->val;
		ir = sol_fa_rule_inspect(rule);
		strcat(i, ir);
	}
	sol_free(ir);
	return i;
}

void sol_dfa_free_inspect(char *i)
{
	sol_free(i);
}


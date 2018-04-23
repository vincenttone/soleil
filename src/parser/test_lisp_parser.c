#include <stdio.h>
#include <string.h>
#include "sol_lisp_parser.h"

size_t solLispParser_symbol_hash1(void *key)
{
    return *(char*)key;
}

size_t solLispParser_symbol_hash2(void *key)
{
	return *(char*)key + 1;
}

int solLispParser_symbol_hash_equal(void *v1, void *v2)
{
	return *(char*)v1 - (*(char*)v2);
}

char* init_symbols(SolLispParser *p, char *symbols, size_t sc)
{
	// symbols
	SolPdaSymbol *left;
	SolPdaSymbol *right;
	SolPdaSymbol *split = solPda_register_symbol_group(p->pda, 0);
	SolPdaSymbol *others = solPda_register_symbol_group(p->pda, 0);
	char i;
	char c;
	for (i = 0; i < sc; i++) {
		c = i + ' ';
		*(char*)((symbols) + i) = c;
		switch (c) {
		case '(':
			left = solPda_register_symbol(p->pda, (symbols + i));
			break;
		case ')':
			right = solPda_register_symbol(p->pda, (symbols + i));
			break;
		case ' ':
			solPda_append_symbol_to_group(p->pda, ((symbols) + i), split);
			break;
		default:
			solPda_append_symbol_to_group(p->pda, ((symbols) + i), others);
		}
	}
	*(char*)((symbols) + 95) = 0x9;
	*(char*)((symbols) + 96) = 0xA;
	solPda_append_symbol_to_group(p->pda, ((symbols) + 95), split);
	solPda_append_symbol_to_group(p->pda, ((symbols) + 96), split);
	// states
	SolPdaState *s1 = solPda_generate_state(p->pda);
	SolPdaState *s2 = solPda_generate_state(p->pda);
	SolPdaState *s3 = solPda_generate_state(p->pda);
	SolPdaState *s4 = solPda_generate_state(p->pda);
	// add rules
	solPda_gen_rules_table(p->pda);
	solPda_add_rule(p->pda, s1, left,   s2, SolPdaFieldFlag_push);
	solPda_add_rule(p->pda, s2, NULL,   s1, 0);
	solPda_add_rule(p->pda, s2, right,  s2, SolPdaFieldFlag_pop);
	solPda_add_rule(p->pda, s2, others, s3, 0);
	solPda_add_rule(p->pda, s2, split,  s4, 0);
	solPda_add_rule(p->pda, s3, NULL,   s2, 0);
	solPda_add_rule(p->pda, s3, others, s3, 0);
	solPda_add_rule(p->pda, s4, split,  s4, 0);
	solPda_add_rule(p->pda, s4, NULL,   s2, 0);
	p->pda->cs = s1;
	p->pda->as = s1;
	return symbols;
}

size_t read_buffer(char *buffer, size_t buffer_size)
{
	if (buffer_size) {
		return 1;
	} else {
		return 0;
	}
}

int main()
{
	SolLispParser *p = solLispParser_new();
	p->read_char = &read_buffer;
	size_t sc = 97;
	char *ss = sol_calloc(sc, sizeof(char)); //symbol list
	solHash_set_hash_func1(p->pda->symbol_map, &solLispParser_symbol_hash1);
	solHash_set_hash_func2(p->pda->symbol_map, &solLispParser_symbol_hash2);
	solHash_set_equal_func(p->pda->symbol_map, &solLispParser_symbol_hash_equal);
	char *symbols = init_symbols(p, ss, sc);
	size_t i;
	printf("Chars (%zu): ", sc);
	for (i = 0; i < sc; i++) {
		printf("[%c] ", *(char*)((symbols) + i));
	}
	printf("\n");

	char *input = "(a b c (1 2 3) m n o)";
	int read = solLispParser_read(p, input, strlen(input));
	if (read != 0) {
		printf("Read return %d\n", read);
	} else {
		printf("Accepting? %d\n", solPda_is_accepting(p->pda));
	}
	sol_free(symbols);
	solLispParser_free(p);
	return 0;
}

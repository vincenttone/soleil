#include <stdio.h>
#include "sol_pda.h"

size_t hash1(void *key)
{
    return *(char*)key;
}

size_t hash2(void *key)
{
	return *(char*)key + 1;
}

int hash_equal(void *v1, void *v2)
{
	return (*(char*)v1) - (*(char*)v2);
}

int state_change(SolPda *pda, SolPdaSymbol *sbl, SolPdaState *s1, SolPdaState *s2, int flag, void *ext)
{
	printf("-> ");
	if (flag & SolPdaFieldFlag_stack_empty) {
		printf("Stack Empty, state: %zu --> %zu\n", s1->state, s2->state);
		return 0;
	}
	printf("Read %c, state: %zu --> %zu\t", *(char*)(sbl->symbol), s1->state, s2->state);
	if (flag & SolPdaFieldFlag_stack_push) {
		printf("Stack Push");
	} else if ((flag & SolPdaFieldFlag_stack_pop)) {
		printf("Stack Pop");
	}
	printf("\n");
	return 0;
}

int main()
{
	char l = '(';
	char r = ')';
	char ca = 'a';
	char cb = 'b';
	char cm = 'm';
	char cn = 'n';
	char co = 'o';
	char bb = ' ';
	SolPda *pda = solPda_new();
	pda->state_change_cb = &state_change;
	solHash_set_hash_func1(pda->symbol_map, &hash1);
	solHash_set_hash_func2(pda->symbol_map, &hash2);
	solHash_set_equal_func(pda->symbol_map, &hash_equal);
	SolPdaState *s1 = solPda_generate_state(pda);
	SolPdaState *s2 = solPda_generate_state(pda);
	SolPdaState *s3 = solPda_generate_state(pda);
	SolPdaState *s4 = solPda_generate_state(pda);

	SolPdaSymbol *sbll = solPda_register_symbol(pda, &l);
	SolPdaSymbol *sblr = solPda_register_symbol(pda, &r);
	SolPdaSymbol *sblw = solPda_register_symbol(pda, &ca);
	SolPdaSymbol *sblcb = solPda_register_symbol(pda, &cb);
	SolPdaSymbol *sblb = solPda_register_symbol(pda, &bb);
	SolPdaSymbol *sblmno = solPda_register_symbol_group(pda, 2, &cm, &cn);
	printf("Append to group return %d\n", solPda_append_symbol_to_group(pda, &co, sblmno));
	printf("Group symbol count %zu\n", solList_len((SolList*)(sblmno->symbol)));

	printf("Gen rules table ret %d\n", solPda_gen_rules_table(pda));
	// rules
	printf("Add rule ret %d\n", solPda_add_rule(pda, s1, sbll, s2, NULL, SolPdaFieldFlag_stack_push));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s1, sblb, s3, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s1, sblw, s4, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s1, sblcb, s4, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s1, sblmno, s4, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s2, NULL, s1, NULL, SolPdaFieldFlag_stack_empty));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s2, sbll, s2, NULL, SolPdaFieldFlag_stack_push));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s2, sblr, s2, sbll, SolPdaFieldFlag_stack_pop));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s2, sblb, s3, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s2, sblw, s4, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s2, sblcb, s4, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s2, sblmno, s4, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s3, NULL, s2, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s4, NULL, s2, NULL, 0));
	/*
	// basic rules
	printf("Add rule ret %d\n", solPda_add_rule(pda, s1, sbll, s2, NULL, SolPdaFieldFlag_stack_push));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s2, sblr, s2, sbll, SolPdaFieldFlag_stack_pop));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s2, sblw, s3, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s2, sblcb, s3, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s2, sblmno, s3, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s2, sblb, s4, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s3, sblw, s3, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s3, sblcb, s3, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s3, sblmno, s3, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s4, sblb, s4, NULL, 0));
	// free moves
	printf("Add rule ret %d\n", solPda_add_rule(pda, s2, NULL, s1, NULL, SolPdaFieldFlag_stack_empty));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s3, NULL, s2, NULL, 0));
	printf("Add rule ret %d\n", solPda_add_rule(pda, s4, NULL, s2, NULL, 0));
	*/

	solPda_init(pda, s1, s1);
	printf("Current state: %zu\tAccepting? %d\n", pda->cs->state, solPda_is_accepting(pda));

	size_t i,j;
	SolPdaField *field;
	printf("Rules Table:\n\t");
	SolListNode *node = solList_head(pda->symbols);
	SolPdaSymbol *sx;
	do {
		sx = solListNode_val(node);
		if (sx->flag & SOL_PDA_SYMBOL_FLAG_GROUP) {
			printf("<<Grp>>\t");
		} else if (sx->flag & SOL_PDA_SYMBOL_FLAG_VIRTUAL) {
			printf("<<V>>\t");
		} else {
			printf("<<%c>>\t", *(char*)(sx->symbol));
		}
	} while ((node = solListNode_next(node)));
	printf("\n");
	for (i = 0; i< solList_len(pda->states); i++) {
		printf("%zu\t", i);
		for (j = 0; j < solList_len(pda->symbols); j++) {
			field = solTableFixed_get(pda->rules, i, j);
			if (field) {
				printf("%zu\t", field->state->state);
			} else {
				printf("NaN\t");
			}
		}
		printf("\n");
	}

	SolPdaSymbol* input[10] = {sbll, sblr, sbll, sbll, sblw, sblb, sblw, sblcb, sblr, sblr};
	for (i = 0; i < 10; i++) {
		if (solPda_read_symbol(pda, input[i], NULL) == 0) {
			printf("Read symbol: [%c] ok.\t", *(char*)((input[i]))->symbol);
		} else {
			printf("Read symbol: [%c] failed!\n", *(char*)((input[i]))->symbol);
			break;
		}
		printf("Current state: %zu\tAccepting? %d\n", pda->cs->state, solPda_is_accepting(pda));
	}
	char *input2 = "(a b a (m n o))";
	printf("Input: %s\n", input2);
	int read;
	for (i = 0; i < 15; i++) {
		read = solPda_read(pda, input2 + i, NULL);
		if (read == 0) {
			printf("Read symbol: [%c] ok.\t", *(char*)(input2 + i));
		} else {
			printf("Read symbol: [%c] failed! return %d\n", *(char*)(input2 +i), read);
			break;
		}
		printf("Current state: %zu, action: %d\tAccepting? %d\n", pda->cs->state, pda->act, solPda_is_accepting(pda));
	}
	solPda_free(pda);
	
	return 0;
}

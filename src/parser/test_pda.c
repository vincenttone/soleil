#include <stdio.h>
#include "sol_pda.h"

int main()
{
	char l = '(';
	char r = ')';
	char ca = 'a';
	char cb = 'b';
	/*
	char cm = 'm';
	char cn = 'n';
	char co = 'o';
	*/
	char bb = ' ';
	SolPda *pda = solPda_new(4, 5);
	SolPdaState *s1 = solPda_generate_state(pda);
	SolPdaState *s2 = solPda_generate_state(pda);
	SolPdaState *s3 = solPda_generate_state(pda);
	SolPdaState *s4 = solPda_generate_state(pda);

	SolPdaSymbol *sbll = solPda_register_symbol(pda, &l);
	SolPdaSymbol *sblr = solPda_register_symbol(pda, &r);
	SolPdaSymbol *sblw = solPda_register_symbol(pda, &ca);
	SolPdaSymbol *sblcb = solPda_register_symbol(pda, &cb);
	SolPdaSymbol *sblb = solPda_register_symbol(pda, &bb);
	//SolPdaSymbol *sblmno = solPda_register_symbol_group(pda, 3, &cm, &cn, &co);

	SolListNode *node = solList_head(pda->symbols);
	SolPdaSymbol *sx;
	printf("symbol counts:\n");
	do {
		sx = solListNode_val(node);
		printf("%zu\t", sx->c);
	} while ((node = solListNode_next(node)));
	printf("\n");

	// basic rules
	solPda_add_rule(pda, s1, sbll, s2, SolPdaFieldFlag_push);
	solPda_add_rule(pda, s2, sblr, s2, SolPdaFieldFlag_pop);
	solPda_add_rule(pda, s2, sblw, s3, 0);
	solPda_add_rule(pda, s3, sblw, s3, 0);
	solPda_add_rule(pda, s3, sblcb, s3, 0);
	solPda_add_rule(pda, s2, sblb, s4, 0);
	solPda_add_rule(pda, s4, sblb, s4, 0);
	// free moves
	solPda_add_rule(pda, s2, NULL, s1, 0);
	solPda_add_rule(pda, s3, NULL, s2, 0);
	solPda_add_rule(pda, s4, NULL, s2, 0);
	
	solPda_init(pda, s1, s1);
	printf("Current state: %zu\tAccepting? %d\n", pda->cs->state, solPda_is_accepting(pda));

	SolPdaSymbol* input[10] = {sbll, sblr, sbll, sbll, sblw, sblb, sblw, sblcb, sblr, sblr};
	int i;
	for (i = 0; i < 10; i++) {
		if (solPda_read(pda, input[i]) == 0) {
			printf("Read symbol: [%c] ok.\t", *(char*)((input[i]))->symbol);
		} else {
			printf("Read symbol: [%c] failed!\t", *(char*)((input[i]))->symbol);
		}
		printf("Current state: %zu\tAccepting? %d\n", pda->cs->state, solPda_is_accepting(pda));
	}
	printf("==========\n");

	solPda_init(pda, s1, s1);
	
	printf("Read symbol: [%c], return %d\n", *(char*)(sblw->symbol), solPda_read(pda, sblw));
	printf("Current state: %zu\tAccepting? %d\n", pda->cs->state, solPda_is_accepting(pda));
	
	printf("Read symbol: [%c], return %d\n", *(char*)(sbll->symbol), solPda_read(pda, sbll));
	printf("Current state: %zu\tAccepting? %d\n", pda->cs->state, solPda_is_accepting(pda));
	
	printf("Read symbol: [%c], return %d\n", *(char*)(sbll->symbol), solPda_read(pda, sbll));
	printf("Current state: %zu\tAccepting? %d\n", pda->cs->state, solPda_is_accepting(pda));

	printf("Read symbol: [%c], return %d\n", *(char*)(sblw->symbol), solPda_read(pda, sblw));
	printf("Current state: %zu\tAccepting? %d\n", pda->cs->state, solPda_is_accepting(pda));

	printf("Read symbol: [%c], return %d\n", *(char*)(sblb->symbol), solPda_read(pda, sblb));
	printf("Current state: %zu\tAccepting? %d\n", pda->cs->state, solPda_is_accepting(pda));

	printf("Read symbol: [%c], return %d\n", *(char*)(sblw->symbol), solPda_read(pda, sblw));
	printf("Current state: %zu\tAccepting? %d\n", pda->cs->state, solPda_is_accepting(pda));

	printf("Read symbol: [%c], return %d\n", *(char*)(sblw->symbol), solPda_read(pda, sblw));
	printf("Current state: %zu\tAccepting? %d\n", pda->cs->state, solPda_is_accepting(pda));
	
	printf("Read symbol: [%c], return %d\n", *(char*)(sblr->symbol), solPda_read(pda, sblr));
	printf("Current state: %zu\tAccepting? %d\n", pda->cs->state, solPda_is_accepting(pda));
	
	printf("Read symbol: [%c], return %d\n", *(char*)(sblr->symbol), solPda_read(pda, sblr));
	printf("Current state: %zu\tAccepting? %d\n", pda->cs->state, solPda_is_accepting(pda));
	
	solPda_free(pda);
	
	return 0;
}

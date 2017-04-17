#include <string.h>
#include "sol_pda.h"

int sm(void *a, void *b)
{
	if ((int*)a == (int*)b) {
		return 0;
	}
	return 1;
}

int cm(void *a, void *b)
{
	return strcmp((char*)a, (char*)b);
}

int main()
{
	int s1 = 1;
	int s2 = 2;
	//int *s3 = 3;
	char *c1 = "a";
	// char *c2 = 'b';
 	//char *c3 = 'c';
	SolPda *p = solPda_new();
	solPda_set_state_match_func(p, sm);
	solPda_set_character_match_func(p, cm);
	solPda_add_rule(p, &s1, &s2, c1);
	solPda_free(p);
	return 1;
}

#include "sol_pda.h"

int main()
{
	int s1 = 1;
	int s2 = 2;
	//int *s3 = 3;
	char *c1 = "a";
	// char *c2 = 'b';
	//char *c3 = 'c';
	SolPda *p = solPda_new();
	solPda_add_rule(p, &s1, &s2, c1);
	solPda_free(p);
	return 1;
}

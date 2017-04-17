#include <string.h>
#include <stdio.h>
#include "sol_utils.h"
#include "sol_pda.h"


int equal(void *, void*);

int equal(void *k1, void *k2)
{
	return solVal_equal((SolVal*)k1, (SolVal*)k2);
}

void x_free(void*);
void x_free(void *s)
{
	if (s) {
		if (((SolVal*)s)->t == SolValTypeInt) {
			printf("try free %d\n", (((SolVal*)s)->v.i));
		}
		if (((SolVal*)s)->t == SolValTypeInt) {
			printf("try free %c\n", (((SolVal*)s)->v.c));
		}
		sol_free(s);
	}
}

int main()
{
	SolVal *s1 = sol_alloc(sizeof(SolVal));
	s1->v.i = 1;
	s1->l = sizeof(int);
	s1->t = SolValTypeInt;
	SolVal *s2 = sol_alloc(sizeof(SolVal));
	s2->v.i = 2;
	s2->l = sizeof(int);
	s2->t = SolValTypeInt;
	//int s3 = 3;
	SolVal *c1 = sol_alloc(sizeof(SolVal));
	c1->v.c = 'a';
	c1->l = sizeof(char);
	c1->t = SolValTypeChar;
	// char *c2 = 'b';
 	//char *c3 = 'c';
	SolPda *p = solPda_new();
	solPda_set_state_free_func(p, &x_free);
	solPda_set_character_free_func(p, &x_free);
	solPda_set_state_match_func(p, &equal);
	solPda_set_character_match_func(p, &equal);
	solPda_add_rule(p, s1, s2, c1);
	solPda_free(p);
	return 1;
}

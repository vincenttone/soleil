#include <string.h>
#include <stdio.h>
#include "sol_utils.h"
#include "sol_nfa.h"


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
		if (((SolVal*)s)->t == SolValTypeChar) {
			printf("try free %c\n", (((SolVal*)s)->v.c));
		}
		sol_free(s);
	} else {
		printf("empty data.\n");
	}
}
void print_pda_states(SolSet *ss);
void print_pda_states(SolSet *ss)
{
	SolNfaState *s;
	printf("states: (");
	size_t c = solSet_count(ss);
	solSet_rewind(ss);
	while ((s = solSet_get(ss))) {
		if (--c > 0) {
			printf("%d, ", ((SolVal*)(s->s))->v.i);
		} else {
			printf("%d", ((SolVal*)(s->s))->v.i);
		}
	}
	printf(")\n");
}

int main()
{
	int r = 0;
	SolVal *s1 = sol_alloc(sizeof(SolVal));
	solVal_int(s1, 1);
	SolVal *s2 = sol_alloc(sizeof(SolVal));
	solVal_int(s2, 2);
	SolVal *s3 = sol_alloc(sizeof(SolVal));
	solVal_int(s3, 3);
	SolVal *s4 = sol_alloc(sizeof(SolVal));
	solVal_int(s4, 4);
	SolVal *s5 = sol_alloc(sizeof(SolVal));
	solVal_int(s5, 5);
	SolVal *s6 = sol_alloc(sizeof(SolVal));
	solVal_int(s6, 6);
	SolVal *c1 = sol_alloc(sizeof(SolVal));
	solVal_char(c1, 'a');
	SolVal *c2 = sol_alloc(sizeof(SolVal));
	solVal_char(c2, 'b');
	SolVal *c3 = sol_alloc(sizeof(SolVal));
	solVal_char(c3, 'c');
	SolNfa *p = solNfa_new();
	solNfa_set_state_free_func(p, &x_free);
	solNfa_set_character_free_func(p, &x_free);
	solNfa_set_state_match_func(p, &equal);
	solNfa_set_character_match_func(p, &equal);
	solNfa_add_rule(p, s1, s2, c1);
	solNfa_add_rule(p, s1, s3, c1);
	solNfa_add_rule(p, s3, s2, c2);
	solNfa_add_rule(p, s2, s4, c3);
	solNfa_add_rule(p, s4, s3, c1);
	solNfa_add_rule(p, s3, s5, NULL);
	solNfa_add_rule(p, s5, s6, c3);
	solNfa_add_current_state(p, s1);
	solNfa_add_accepting_state(p, s5);
	if (solNfa_is_accepted(p) == 0) printf("accepted!\n");
	printf("Accepting states:\n");
	print_pda_states(solNfa_accepting_states(p));
	printf("Reading character:\n");
	print_pda_states(solNfa_current_states(p));
	r = solNfa_read_character(p, c1);
	if (r != 0) goto end;
	print_pda_states(solNfa_current_states(p));
	if (solNfa_is_accepted(p) == 0) printf("accepted!\n");

	r = solNfa_read_character(p, c2);
	if (r != 0) goto end;
	print_pda_states(solNfa_current_states(p));
	if (solNfa_is_accepted(p) == 0) printf("accepted!\n");

	r = solNfa_read_character(p, c3);
	if (r != 0) goto end;
	print_pda_states(solNfa_current_states(p));
	if (solNfa_is_accepted(p) == 0) printf("accepted!\n");
	goto finish;
 end:
	printf("something wrong\n");
 finish:
	if (solNfa_is_accepted(p) == 0) {
		solSet_rewind(p->cs);
		SolSet* ass = solSet_get_intersection(p->cs, p->as);
		printf("accepted states:\n");
		print_pda_states(ass);
		solSet_free(ass);
	} else {
		printf("not accepted!\n");
	}
	solNfa_free(p);
	return r;
}

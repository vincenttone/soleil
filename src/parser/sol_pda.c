#include "sol_pda.h"

SolPdaStates* solPdaStates_new()
{
	SolPdaStates *ps = sol_alloc(sizeof(SolPdaStates));
	if (ps == NULL) {
		return NULL;
	}
	ps->n = NULL;
	ps->f = NULL;
	return ps;
}

void solPdaStates_free(SolPdaStates *ps)
{
	if (ps->n) {
		sol_hash_free(p->n);
	}
	if (ps->f) {
		solSet_free(p->f);
	}
	if (ps) {
		sol_free(ps);
	}
}

SolPdaStates* solPdaStates_add_rule(SolPdaStates *ps, void *s1, void *s2, void *c)
{
	solPdaStates_set_state(ps, s1);
	SolPdaStates *psn = solPdaStates_new();
	solPdaStates_set_state(psn, s2);
	if (c == NULL) {
		if (ps->f == NULL) {
			ps->f = solSet_new();
		}
		if (solSet_add(ps->f, psn) == 0) {
			return psn;
		} else {
			return NULL;
		}
	} else {
		if (ps->n == NULL) {
			ps->n = sol_hash_new();
		}
		if (sol_hash_put(ps->n, c, psn) == 0) {
			return psn;
		} else {
			return NULL;
		}
	}
}

void* solPdaStates_get_next_states(solPdaStates *ps, void *c)
{
	if (c == NULL) {
		return ps->f;
	} else {
		return sol_hash_get(ps->n, c);
	}
}

SolPda* solPda_new()
{
	SolPda *p = sol_alloc(sizeof(solPda));
	p->s = solPdaStates_new();
	p->l = solList_new();
	return p;
}

void solPda_free(SolPda *p)
{
	solPdaStates_free(p->s);
	solList_free(p->l);
	sol_free(p);
}

int solPda_add_rule(SolPda *p, void *s1, void *s2, void *c)
{
	SolListIter *i = solListIter_new(p->l);
	SolListNode *n;
	SolPdaStates *ps;
	while (n = solListIter_next(i)) {
		if (p->sm(n->val, s1) == 0) {
			ps = solPdaStates_add_rule(n->val->s, s1, s2, c);
			if (ps) {
				if (solList_add(p->l, ps) == NULL) {
					return -2;
				}
			} else {
				return -1;
			}
			return 0;
		}
	}
	return 1;
}

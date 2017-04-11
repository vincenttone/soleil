#include "sol_pda.h"

SolPdaState* solPdaState_new(void *s)
{
	SolPdaState *ps = sol_alloc(sizeof(SolPdaState));
	if (ps == NULL) {
		return NULL;
	}
	ps->s = s;
	ps->n = NULL;
	ps->f = NULL;
	return ps;
}

void solPdaState_free(SolPdaState *ps)
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

int solPdaState_add_rule(SolPdaState *ps, SolPdaState *ns, void *c)
{
	if (c == NULL) {
		if (ps->f == NULL) {
			ps->f = solSet_new();
		}
		if (solSet_add(ps->f, psn) == 0) {
			return 0;
		} else {
			return 1;
		}
	} else {
		if (ps->n == NULL) {
			ps->n = sol_hash_new();
		}
		if (ps->n == NULL) {
			return 2;
		}
		if (sol_hash_put(ps->n, c, ns) == 0) {
			return 0;
		} else {
			return 3;
		}
	}
}

void* solPdaState_get_next_states(solPdaState *ps, void *c)
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
	p->s = NULL;
	p->l = solList_new();
	p->li = solListIter_new(p->l);
	solList_set_free_func(p->l, &solPdaState_free);
	return p;
}

void solPda_free(SolPda *p)
{
	solListIter_free(p->li);
	solList_free(p->l);
	sol_free(p);
}

int solPda_add_rule(SolPda *p, void *s1, void *s2, void *c)
{
	int rtn = 1;
	SolPdaState *ps1 = NULL;
	SolPdaState *ps2 = NULL;
	SolListNode *n;
	solListIter_rewind(p->li);
	while (n = solListIter_next(p->li)) {
		if (ps1 != NULL && p->f_sm(n->val, s1) == 0) {
			ps1 = n->val;
		}
		if (ps2 != NULL && p->f_sm(n->val, s2) == 0) {
			ps2 = n->val;
		}
	}
	if (ps1 == NULL) {
		ps1 = solPdaState_new(s1);
		if (ps1 == NULL) {
			return 10;
		}
	}
	if (ps2 == NULL) {
		ps2 = solPdaState_new(s2);
		if (ps2 == NULL) {
			return 11;
		}
	}
	return solPdaState_add_rule(ps1, ps2, c);
}

void* solPda_next_states(SolPda *p, void* c)
{
	if (c == NULL) {
		return p->f;
	} else {
		if (p->s && ps->s->n && sol_hash_has_key(p->s->n, c)) {
			return sol_hash_get(p->l, c);
		}
	}
	return NULL;
}

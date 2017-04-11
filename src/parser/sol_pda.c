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
	p->s = NULL;
	p->l = solList_new();
	p->li = solListIter_new(p->l);
	solList_set_free_func(p->l, &solPdaStates_free);
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
	SolPdaStates *ps;
	if (p->s == NULL) {
		p->s = solPdaStates_new();
		if (p->s) {
			ps = solPdaStates_add_rule(p->s, s1, s2, c);
			if (ps) {
				if (solList_add(p->l, p->s)	&& solList_add(p->, ps)) {
						rtn = 0;
						goto finish;
				}
				rtn = -2;
			} else {
				rtn = -1;
			}
		} else {
			rtn = -1;
		}
		goto err;
	}
	solListIter_rewind(p->li);
	SolListNode *n;
	while (n = solListIter_next(p->li)) {
		if (p->f_sm(n->val, s1) == 0) {
			ps = solPdaStates_add_rule(n->val->s, s1, s2, c);
			if (ps) {
				if (solList_add(p->l, ps) == NULL) {
					rtn = -2;
					goto err;
				}
			} else {
				rtn = -1;
				goto err;
			}
			goto finish;
		}
	}
 err:
 finish:
	return rtn;
}

void* solPda_next_states(SolPda *p, void* c)
{
	if (c == NULL) {
		return p->f;
	} else {
		if (sol_hash_has_key(p->l, c)) {
			return sol_hash_get(p->l, c);
		}
	}
	return NULL;
}

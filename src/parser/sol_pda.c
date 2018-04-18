#include "sol_pda.h"

SolPda* solPda_new(size_t state_count, size_t symbol_count)
{
	SolPda *pda = sol_calloc(1, sizeof(SolPda));
	if (pda == NULL) {
		return NULL;
	}
	pda->fields = solList_new();
	pda->states = solList_new();
	pda->rules = solTableFixed_new(state_count, symbol_count);
	pda->stk = solStack_new();
	return pda;
}

void solPda_free(SolPda *pda)
{
	SolListNode *n;
	if (pda->fields && solList_len(pda->fields)) {
		n = solList_head(pda->fields);
		SolPdaField *f;
		do {
			f = (SolPdaField*)(solListNode_val(n));
			if (f) {
				solPdaField_free(f);
			}
		} while ((n = solListNode_next(n)));
	}
	if (pda->states && solList_len(pda->states)) {
		n = solList_head(pda->states);
		do {
			solPdaState_free((SolPdaState*)(solListNode_val(n)));
		} while ((n = solListNode_next(n)));
	}
	if (pda->stk) {
		solStack_free(pda->stk);
	}
	if (pda->rules) {
		solTableFixed_free(pda->rules);
	}
	if (pda) {
		sol_free(pda);
	}
}

void solPda_init(SolPda *pda, SolPdaState *start, SolPdaState *accept)
{
	pda->cs = start;
	pda->as = accept;
	while (solStack_pop(pda->stk)) {}
}

int solPda_add_rule(SolPda *pda, SolPdaState *s1, SolPdaSymbol *sbl, SolPdaState *s2, int act)
{
	if (sbl == NULL) { // free moves
		if (s1->free_moves == NULL) {
			s1->free_moves = solList_new();
			if (s1->free_moves == NULL) {
				return -1;
			}
		}
		solList_add(s1->free_moves, s2);
	} else {
		SolPdaField *fs = solPdaField_new(s2, act);
		if (solTableFixed_put(pda->rules, s1->state, sbl->c, fs)) {
			return 1;
		}
		if (solList_add(pda->fields, fs) == NULL) {
			solPdaField_free(fs);
			return -2;
		}
	}
	return 0;
}

int solPda_read(SolPda *pda, SolPdaSymbol *sbl)
{
	SolPdaField *fs = solTableFixed_get(pda->rules, pda->cs->state, sbl->c);
	if (fs == NULL) {
		// free moves
		if (pda->cs && pda->cs->free_moves && solList_len(pda->cs->free_moves)) {
			SolListNode *n = solList_head(pda->cs->free_moves);
			SolPdaState *s;
			do {
				s = solListNode_val(n);
				fs = solTableFixed_get(pda->rules, s->state, sbl->c);
				if (fs) {
					break;
				}
			} while ((n = solListNode_next(n)));
		}
	}
	if (fs == NULL) {
		return 1;
	}
	pda->cs = fs->state;
	if ((fs->flag & SolPdaFieldFlag_push)) {
		if (solStack_push(pda->stk, sbl)) {
			return 2;
		}
	} else if (fs->flag & SolPdaFieldFlag_pop) {
		solStack_pop(pda->stk);
	}
	return 0;
}

int solPda_is_accepting(SolPda *pda)
{
	if (pda == NULL) {
		return -1;
	}
	if (solStack_size(pda->stk)) {
		return 1;
	}
	if (solPda_check_state_accepting(pda, pda->cs))	 {
		return 2;
	}
	return 0;
}

int solPda_check_state_accepting(SolPda *pda, SolPdaState *s)
{
	if (pda == NULL || s == NULL) {
		return -1;
	}
	if (pda->as == s) {
		return 0;
	}
	if (pda->lc++ > SOL_PDA_MAX_LOOP_COUNT) {
		return -2;
	}
	if (s->free_moves && solList_len(s->free_moves)) {
		SolListNode *n = solList_head(s->free_moves);
		do {
			if (solPda_check_state_accepting(pda, (SolPdaState*)(solListNode_val(n))) == 0) {
				return 0;
			}
		} while ((n = solListNode_next(n)));
	}
	return 1;
}

SolPdaState* solPda_generate_state(SolPda *pda)
{
	SolPdaState *s = sol_calloc(1, sizeof(SolPdaState));
	if (s == NULL) {
		return NULL;
	}
	if (solList_len(pda->states) == 0) {
		s->state = 1;
	} else {
		SolPdaState *ls = solListNode_val(solList_tail(pda->states));
		s->state = ls->state + 1;
	}
	if (solList_add(pda->states, s) == NULL) {
		solPdaState_free(s);
		return NULL;
	}
	return s;
}

SolPdaSymbol* solPda_register_symbol(SolPda *pda, void *symbol)
{
	SolPdaSymbol *s = sol_alloc(sizeof(SolPdaSymbol));
	if (s == NULL) {
		return NULL;
	}
	s->symbol = symbol;
	s->c = pda->g2++;
	return s;
}

void solPdaState_free(SolPdaState *s)
{
	if (s) {
		if (s->free_moves) {
			solList_free(s->free_moves);
		}
		sol_free(s);
	}
}

SolPdaField* solPdaField_new(SolPdaState *s, int flag)
{
	SolPdaField *f = sol_alloc(sizeof(SolPdaField));
	if (f == NULL) {
		return NULL;
	}
	f->flag = flag;
	f->state = s;
	return f;
}

void solPdaField_free(SolPdaField *f)
{
	if (f) {
		sol_free(f);
	}
}

#include "sol_pda.h"

SolPda* solPda_new()
{
	SolPda *pda = sol_calloc(1, sizeof(SolPda));
	if (pda == NULL) {
		return NULL;
	}
	pda->fields = solList_new();
	pda->states = solList_new();
	pda->symbols = solList_new();
	//pda->rules = solTableFixed_new(state_count, symbol_count);
	pda->stk = solStack_new();
	pda->symbol_map = solHash_new();
	return pda;
}

int solPda_gen_rules_table(SolPda *pda)
{
	if (pda == NULL) {
		return -1;
	}
	if (pda->rules) {
		solTableFixed_free(pda->rules);
	}
	pda->rules = solTableFixed_new(solList_len(pda->states), solList_len(pda->symbols));
	if (pda->rules == NULL) {
		return 1;
	}
	return 0;
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
	if (pda->symbols && solList_len(pda->symbols)) {
		n = solList_head(pda->symbols);
		do {
			solPdaSymbol_free((SolPdaSymbol*)(solListNode_val(n)));
		} while ((n = solListNode_next(n)));
	}
	if (pda->stk) {
		solStack_free(pda->stk);
	}
	if (pda->rules) {
		solTableFixed_free(pda->rules);
	}
	if (pda->symbol_map) {
		solHash_free(pda->symbol_map);
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
		if ((s1->state + 1 > pda->rules->cs)
			|| (sbl->c + 1 > pda->rules->rs)
		) {
			return -2;
		}
		SolPdaField *fs = solPdaField_new(s2, act);
		if (solTableFixed_put(pda->rules, s1->state, sbl->c, fs)) {
			return 1;
		}
		if (solList_add(pda->fields, fs) == NULL) {
			solPdaField_free(fs);
			return -3;
		}
	}
	return 0;
}

int solPda_read(SolPda *pda, void *s)
{
	SolPdaSymbol *symbol = solHash_get(pda->symbol_map, s);
	if (symbol == NULL) {
		return -1;
	}
	return solPda_read_symbol(pda, symbol);
}

int solPda_read_symbol(SolPda *pda, SolPdaSymbol *sbl)
{
	SolPdaField *fs = solTableFixed_get(pda->rules, pda->cs->state, sbl->c);
	if (fs == NULL) {
		pda->lc = 0;
		fs = solPda_free_moves_find(pda, pda->cs, sbl);
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

SolPdaField* solPda_free_moves_find(SolPda *pda, SolPdaState *s, SolPdaSymbol *sbl)
{
	if (pda->lc++ > SOL_PDA_MAX_LOOP_COUNT) {
		return NULL;
	}
	SolPdaField *fs;
	SolListNode *n;
	if (s && s->free_moves && solList_len(s->free_moves)) {
		n = solList_head(s->free_moves);
		do {
			s = solListNode_val(n);
			fs = solTableFixed_get(pda->rules, s->state, sbl->c);
			if (fs) {
				return fs;
			}
			fs = solPda_free_moves_find(pda, s, sbl);
			if (fs) {
				return fs;
			}
		} while ((n = solListNode_next(n)));
	}
	return NULL;
}

int solPda_is_accepting(SolPda *pda)
{
	if (pda == NULL) {
		return -1;
	}
	if (solStack_size(pda->stk)) {
		return 1;
	}
	pda->lc = 0;
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
		pda->cs = s;
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
	s->state = solList_len(pda->states);
	if (solList_add(pda->states, s) == NULL) {
		solPdaState_free(s);
		return NULL;
	}
	return s;
}

SolPdaSymbol* solPda_register_symbol(SolPda *pda, void *symbol)
{
	SolPdaSymbol *s = sol_calloc(1, sizeof(SolPdaSymbol));
	if (s == NULL) {
		return NULL;
	}
	s->symbol = symbol;
	s->c = solList_len(pda->symbols);
	if (solHash_put(pda->symbol_map, symbol, s)) {
		solPdaSymbol_free(s);
		return NULL;
	}
	SolListNode *n = solList_add(pda->symbols, s);
	if (n == NULL) {
		solPdaSymbol_free(s);
		return NULL;
	}
	return s;
}

SolPdaSymbol* solPda_register_symbol_group(SolPda *pda, size_t count, ...)
{
	SolPdaSymbol *s = sol_calloc(1, sizeof(SolPdaSymbol));
	if (s == NULL) {
		return NULL;
	}
	s->c = solList_len(pda->symbols);
	if (count == 0) {
		s->symbol = solList_new();
		s->flag = SOL_PDA_SYMBOL_FLAG_GROUP;
		goto end;
	}
	va_list al;
	va_start(al, count);
	if (count == 1) {
		s->symbol = va_arg(al, void*);
		if (solHash_put(pda->symbol_map, s->symbol, s)) {
			solPdaSymbol_free(s);
			return NULL;
		}
	} else {
		s->symbol = solList_new();
		s->flag = SOL_PDA_SYMBOL_FLAG_GROUP;
		void *symbol;
		for (; count > 0; count--) {
			symbol = va_arg(al, void*);
			if (solList_add(s->symbol, symbol) == NULL) {
				solPdaSymbol_free(s);
				return NULL;
			}
			if (solHash_put(pda->symbol_map, symbol, s)) {
				solPdaSymbol_free(s);
				return NULL;
			}
		}
	}
	va_end(al);
 end:
	if (solList_add(pda->symbols, s) == NULL) {
		solPdaSymbol_free(s);
		return NULL;
	}
	return s;
}

int solPda_append_symbol_to_group(SolPda *pda, void *s, SolPdaSymbol *sbl)
{
	if (solHash_get(pda->symbol_map, s)) {
		return 0;
	}
	if (solList_add(sbl->symbol, s) == NULL) {
		return 1;
	}
	if (solHash_put(pda->symbol_map, s, sbl)) {
		return 2;
	}
	return 0;
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

void solPdaSymbol_free(SolPdaSymbol *s)
{
	if (s) {
		if ((s->flag & SOL_PDA_SYMBOL_FLAG_GROUP) && s->symbol) {
			solList_free((SolList*)(s->symbol));
		}
		sol_free(s);
	}
}

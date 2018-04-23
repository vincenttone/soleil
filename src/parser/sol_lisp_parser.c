#include "sol_lisp_parser.h"

SolLispParser* solLispParser_new()
{
	SolLispParser *p = sol_calloc(1, sizeof(SolLispParser));
	// init symbols
	p->symbols = sol_calloc(4, sizeof(SolLispParserSymbol));
	((SolLispParserSymbol*)(p->symbols))->s = SolLispParserSymbolVal_lc;
	((SolLispParserSymbol*)(p->symbols + 1))->s = SolLispParserSymbolVal_rc;
	((SolLispParserSymbol*)(p->symbols + 2))->s = SolLispParserSymbolVal_blank;
	((SolLispParserSymbol*)(p->symbols + 3))->s = SolLispParserSymbolVal_other;
	// create PDA
	p->pda = solPda_new();
	solHash_set_hash_func1(p->pda->symbol_map, &solLispParser_symbol_hash1);
	solHash_set_hash_func2(p->pda->symbol_map, &solLispParser_symbol_hash2);
	solHash_set_equal_func(p->pda->symbol_map, &solLispParser_symbol_hash_equal);
	// register symbols
	SolPdaSymbol *lc = solPda_register_symbol(p->pda, ((SolLispParserSymbol*)(p->symbols)));
	SolPdaSymbol *rc = solPda_register_symbol(p->pda, ((SolLispParserSymbol*)(p->symbols + 1)));
	SolPdaSymbol *blank = solPda_register_symbol(p->pda, ((SolLispParserSymbol*)(p->symbols + 2)));
	SolPdaSymbol *other = solPda_register_symbol(p->pda, ((SolLispParserSymbol*)(p->symbols + 3)));
	// generate state
	// states
	SolPdaState *s1 = solPda_generate_state(p->pda);
	SolPdaState *s2 = solPda_generate_state(p->pda);
	SolPdaState *s3 = solPda_generate_state(p->pda);
	SolPdaState *s4 = solPda_generate_state(p->pda);
	// add rules
	solPda_gen_rules_table(p->pda);
	solPda_add_rule(p->pda, s1, lc,    s2, NULL, SolPdaFieldFlag_push);
	solPda_add_rule(p->pda, s2, NULL,  s1, NULL, 0);
	solPda_add_rule(p->pda, s2, rc,    s2, lc,   SolPdaFieldFlag_pop);
	solPda_add_rule(p->pda, s2, other, s3, NULL, 0);
	solPda_add_rule(p->pda, s2, blank, s4, NULL, 0);
	solPda_add_rule(p->pda, s3, NULL,  s2, NULL, 0);
	solPda_add_rule(p->pda, s3, other, s3, NULL, 0);
	solPda_add_rule(p->pda, s4, blank, s4, NULL, 0);
	solPda_add_rule(p->pda, s4, NULL,  s2, NULL, 0);
	p->pda->cs = s1;
	p->pda->as = s1;
	return p;
}

int solLispParser_read(SolLispParser *p, char *buffer, size_t buffer_size)
{
	if (p == NULL || buffer == NULL || p->read_char == NULL) {
		return -1;
	}
	size_t n;
	char *current = buffer;
	enum SolLispParserSymbolVal sv;
	SolLispParserSymbol *symbol;
	while (buffer_size > 0) {
		n = (*p->read_char)(current, buffer_size);
		if (n <= 0) {
			return 1;
		}
		sv = (*p->get_symbol_val)(current, n);
		if (SolLispParserSymbolVal_max <= sv) {
			return 2;
		}
		symbol = p->symbols + sv - 1;
		if (solPda_read(p->pda, symbol)) {
			return 3;
		}
		current += n;
		buffer_size -= n;
	}
	return 0;
}

void solLispParser_free(SolLispParser *p)
{
	if (!p) return;
	if (p->symbols) {
		sol_free(p->symbols);
	}
	if (p->pda) {
		solPda_free(p->pda);
	}
	sol_free(p);
}

size_t solLispParser_symbol_hash1(void *v)
{
	return ((SolLispParserSymbol*)v)->s;
}

size_t solLispParser_symbol_hash2(void *v)
{
	return ((SolLispParserSymbol*)v)->s + 1;
}

int solLispParser_symbol_hash_equal(void *v1, void *v2)
{
	return ((SolLispParserSymbol*)v1)->s - ((SolLispParserSymbol*)v2)->s;
}

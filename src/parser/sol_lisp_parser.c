#include "sol_lisp_parser.h"

SolLispParser* solLispParser_new()
{
	SolLispParser *p = sol_alloc(sizeof(SolLispParser));
	p->pda = solPda_new();
	p->flag = SolLispParserSymbolType_char;
	return p;
}

int solLispParser_read(SolLispParser *p, char *buffer, size_t buffer_size)
{
	SolLispParserChar *c;
	if ((p->flag & SolLispParserSymbolType_char) == 0) {
		c = sol_alloc(sizeof(SolLispParserChar));
		c->len = 0;
	}
	size_t n;
	char *current = buffer;
	while (buffer_size > 0) {
		n = (*p->read_char)(current, buffer_size);
		if (n <= 0) {
			return 1;
		}
		if (p->flag & SolLispParserSymbolType_char) {
			if (solPda_read(p->pda, current)) {
				return 2;
			}
		} else {
			c->len = n;
			c->c = current;
			if (solPda_read(p->pda, c)) {
				return 2;
			}
		}
		current += n;
		buffer_size -= n;
	}
	return 0;
}

void solLispParser_free(SolLispParser *p)
{
	if (!p) return;
	if (p->pda) {
		solPda_free(p->pda);
	}
	sol_free(p);
}

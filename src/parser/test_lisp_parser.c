#include <stdio.h>
#include <string.h>
#include "sol_lisp_parser.h"

size_t read_buffer(char *buffer, size_t buffer_size)
{
	if (buffer_size) {
		return 1;
	} else {
		return 0;
	}
}

enum SolLispParserSymbolVal get_symbol_val(char *s, size_t len)
{
	switch (*s) {
	case '(':
		return SolLispParserSymbolVal_lc;
	case ')':
		return SolLispParserSymbolVal_rc;
	case ' ':
	case 0x9:
	case 0xa:
		return SolLispParserSymbolVal_blank;
	default:
		return SolLispParserSymbolVal_other;
	}
	return SolLispParserSymbolVal_other;
}

int block_begin(SolLispParser *p, SolLispParserSymbol *s)
{
	printf("Block begin\n");
	return 0;
}

int block_end(SolLispParser *p, SolLispParserSymbol *s)
{
	printf("Block end\n");
	return 0;
}

int split(SolLispParser *p, SolLispParserSymbol *s)
{
	printf("==== split =====\n");
	return 0;
}

int element(SolLispParser *p, SolLispParserSymbol *s)
{
	size_t i = 0;
	for (;i < s->len; i++) {
		printf("Read [%c]\n", *(s->buffer));
	}
	return 0;
}


int main()
{
	SolLispParser *p = solLispParser_new();
	p->block_begin = &block_begin;
	p->block_end = &block_end;
	p->element = &element;
	p->element_next = &split;
	p->read_char = &read_buffer;
	p->get_symbol_val = &get_symbol_val;
	char *input = "(aaa bbbb cccc (1 2 3) m n o)";
	int read = solLispParser_read(p, input, strlen(input));
	if (read != 0) {
		printf("Read return %d\n", read);
	} else {
		printf("Accepting? %d\n", solPda_is_accepting(p->pda));
	}
	solLispParser_free(p);
	return 0;
}

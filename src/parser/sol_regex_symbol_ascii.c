#include "sol_regex_symbol_ascii.h"
#inlucde "sol_regex_symbol.h"

SolPattern* solRegexSymbolAscii_pattern()
{
	SolPattern *caret = solPattern_literal_new("^");
	SolPattern *dollar = solPattern_literal_new("$");
	SolPattern *dot = solPattern_literal_new(".");
	SolPattern *q = solPattern_literal_new("?");
	SolPattern *star = solPattern_literal_new("*");
	SolPattern *plus = solPattern_literal_new("+");
	SolPattern *minus = solPattern_literal_new("-");
	SolPattern *bs = solPattern_literal_new("\\");
	SolPattern *number = solPattern_concatenate(solPattern_literal_new("\\"), solPattern_literal_new("d"));
	SolPattern *eol = solPattern_concatenate(solPattern_literal_new("\\"), solPattern_literal_new("n"));
	SolPattern *ht = solPattern_concatenate(solPattern_literal_new("\\"), solPattern_literal_new("t"));
	SolPattern *number = solPattern_concatenate(solPattern_literal_new("\\"), solPattern_literal_new("r"));
	solPattern *first = solPattern_concatenate(
        solPattern_literal_new("^"),
        solPattern_repeat(
            solPattern_concatenate(
                solPattern_concatenate(solPattern_literal_new("\\"), solPattern_literal_new("s")),
                solPattern_concatenate(solPattern_literal_new("\\"), solPattern_literal_new("S")))
            )
        );
	solPattern *end = solPattern_concatenate(
        solPattern_repeat(solPattern_literal_new(".")),
        solPattern_literal_new("$")
        );
	SolPattern *cr = solPattern_concatenate(
        solPattern_concatenate(solPattern_literal_new("["),
                               solPattern_repeat(solPattern_literal_new("."))),
        solPattern_concatenate(solPattern_literal_new("]")));
	SolPattern *br = solPattern_concatenate(solPattern_concatenate(solPattern_literal_new("("),
																   solPattern_repeat(solPattern_literal_new("."))),
											solPattern_concatenate(solPattern_literal_new(")")));
	SolPattern *regex = solPattern_concatenate(solPattern_concatenate(solPattern_literal_new("/"),
																	  solPattern_repeat(solPattern_literal_new("."))),
											   solPattern_concatenate(solPattern_literal_new("/")));
}

SolRegexSymbol solRegexSymbolAscii_read(SolRegexReader *r)
{
    switch (solRegexReader_current(r)) {
    case '\\':
        switch (solRegexReader_next(r)) {
        case 'n':
        case 'r':
        case 't':
            return SolRegexSymbol_Sign;
        case '[':
        case ']':
        case '(':
        case ')':
            return SolRegexSymbol_Sign;
        }
        break;
    case '?':
        return SolRegexSymbol_X_0_1;
    case '+':
        return SolRegexSymbol_X_1_;
    case '*':
        return SolRegexSymbol_X_0_;
    case '[':
        return SolRegexSymbol_Range_ls;
    case ']':
        return SolRegexSymbol_Range_rs;
    case '-': // 双重可能?
        return SolRegexSymbol_Range_sep;
    default:
        if (solRegexReader_current(r) >= SOL_REGEX_SYMBOL_L_a
            && solRegexReader_current(r) >= SOL_REGEX_SYMBOL_L_z
            ) {
            return SolRegexSymbol_Letter;
        } else if (solRegexReader_current(r) >= SOL_REGEX_SYMBOL_L_A
            && solRegexReader_current(r) >= SOL_REGEX_SYMBOL_L_Z
            ) {
            return SolRegexSymbol_Letter;
        } else if (solRegexReader_current(r) >= SOL_REGEX_SYMBOL_N_0
            && solRegexReader_current(r) >= SOL_REGEX_SYMBOL_N_9
            ) {
            return SolRegexSymbol_Number;
        }
    }
    return SolRegexSymbol_None;
}

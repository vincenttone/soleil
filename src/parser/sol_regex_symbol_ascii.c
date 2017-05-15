#include "sol_regex_symbol_ascii.h"

// [A-Z0-6j-nxz]+abc*
// w{3}\.xyz\.com(/.*)?
// \x [] () {} ? * + .
/**
 * RE := ^RE
 * RE := RE$
 * RE := (RE)
 * RE := RE COUNT
 * RE := RE LIST
 * RE := LIST RE
 * RE := LIST
 * LIST := [ LIST ]
 * LIST := RANGE LIST
 * LIST := LIST RANGE
 * LIST := LIST literal
 * LIST := literal LIST
 * LIST := literal
 * LIST := RANGE
 * RANGE := literal - literal
 * COUNT := *
 * COUNT := ?
 * COUNT := +
 * COUNT := { number }
 * COUNT := { number , }
 * COUNT := { number , number }
 */
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

#ifndef _SOL_REGEX_H_
#define _SOL_REGEX_H_ 1

#include "sol_common.h"
#include "sol_pattern.h"
#include "sol_regex_symbol_ascii.h"

enum SolRegexSymbol {
    SolRegexSymbol_Number = 1,
    SolRegexSymbol_Letter,
    SolRegexSymbol_Empty_or_once, // ? once or empty
    SolRegexSymbol_Repeat_or_empty, // * repeat or empty
    SolRegexSymbol_Repeat, // + repeat once or more
    SolRegexSymbol_Capture_ls, // capture left sign
    SolRegexSymbol_Capture_rs, // capture right sign
    SolRegexSymbol_List_ls, // list left sign
    SolRegexSymbol_List_rs, // list right sign
    SolRegexSymbol_Range_ls, // range left sign
    SolRegexSymbol_Range_sep, // range mid separate
    SolRegexSymbol_Range_rs, // range right sign
    SolRegexSymbol_abbr_sep, // range mid separate
};

typedef enum _SolRegexEncoding {
    SolRegexEncode_ascii = 1,
    SolRegexEncode_utf8 = 2,
} SolRegexEncoding;

typedef struct _SolRegexEngine {
    SolPatternStateGen *g;
    SolRegexEncoding ec;
} SolRegexEngine;

SolRegexEngine* solRegexEngine_new(SolRegexEncoding);
void solRegexEngine_free(SolRegexEngine*);

SolPattern* solRegexEngine_regex_pattern();

SolPattern* solRegexEngine_letter_pattern();
SolPattern* solRegexEngine_number_pattern();

SolPattern* solRegexEngine_character_pattern();
SolPattern* solRegexEngine_list_pattern();
SolPattern* solRegexEngine_capture_pattern();
SolPattern* solRegexEngine_range_pattern();
SolPattern* solRegexEngine_count_pattern();

SolPattern* solRegexEngine_numbers_abbr_pattern();
SolPattern* solRegexEngine_letters_abbr_pattern();

SolPattern* solRegexEngine_conv_pattern(SolRegexEngine*, void*);

#endif

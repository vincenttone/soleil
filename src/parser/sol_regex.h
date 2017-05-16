#ifndef _SOL_REGEX_H_
#define _SOL_REGEX_H_ 1

#include "sol_common.h"
#include "sol_pattern.h"
#include "sol_regex_symbol_ascii.h"

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

SolPattern* solRegexEngine_conv_pattern(SolRegexEngine*, void*);

SolPattern* solRegexEngine_regex_pattern();
SolPattern* solRegexEngine_letter_pattern();
SolPattern* solRegexEngine_number_pattern();
SolPattern* solRegexEngine_character_pattern();
SolPattern* solRegexEngine_list_pattern();
SolPattern* solRegexEngine_capture_pattern();
SolPattern* solRegexEngine_range_pattern();
solPattern* solRegexEngine_count_pattern();
SolPattern* solRegexEngine_numbers_abbr_pattern();
SolPattern* solRegexEngine_letters_abbr_pattern();

#endif

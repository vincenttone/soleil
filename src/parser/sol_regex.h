#ifndef _SOL_REGEX_H_
#define _SOL_REGEX_H_ 1

#include "sol_common.h"
#include "sol_pattern.h"
#include "sol_regex_symbol.h"
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

SolPattern* solRegexEngine_pattern_regex();

SolPattern* solRegexEngine_pattern_letter();
SolPattern* solRegexEngine_pattern_number();

SolPattern* solRegexEngine_pattern_X_0_();
SolPattern* solRegexEngine_pattern_X_1_();
SolPattern* solRegexEngine_pattern_X_0_1();

SolPattern* solRegexEngine_pattern_list();
SolPattern* solRegexEngine_pattern_capture();
SolPattern* solRegexEngine_pattern_range();
SolPattern* solRegexEngine_pattern_group();
SolPattern* solRegexEngine_pattern_not_greed();

SolPattern* solRegexEngine_pattern_character();
SolPattern* solRegexEngine_pattern_count();
SolPattern* solRegexEngine_pattern_numbers_abbr();
SolPattern* solRegexEngine_pattern_letters_abbr();

SolPattern* solRegexEngine_conv_pattern(SolRegexEngine*, void*);

#endif

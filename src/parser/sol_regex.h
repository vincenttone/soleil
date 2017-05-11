#ifndef _SOL_REGEX_H_
#define _SOL_REGEX_H_ 1

#include "sol_common.h"
#include "sol_pattern.h"

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

SolPattern* solRegexEngine_conv_pattern(SolRegexEngine*, char*);

#endif

#include "sol_regex.h"

SolRegexEngine* solRegexEngine_new(SolRegexEncoding *ec)
{
    SolRegexEngine *re = sol_alloc(sizeof(SolRegexEngine));
    if (re == NULL) {
        return NULL;
    }
    re->g = solPatternStateGen_new();
    if (re->g == NULL) {
        solRegexEngine_free(re);
        return NULL;
    }
    re->ec = ec;
    return re;
}

void solRegexEngine_free(SolRegexEngine *re)
{
    if (re->g) {
        solPatternStateGen_free(re->g);
    }
    if (re) {
        sol_free(re);
    }
}

SolPattern* solRegexEngine_character()
{
    return NULL;
}

SolPattern* solRegexEngine_characters()
{
    return solPattern_repeat(solRegexEngine_character());
}

SolPattern* solRegexEngine_list()
{
    SolPattern *l = solPattern_concatenate(
        solPattern_concatenate(
            solPattern_literal_new(SOL_REGEX_SYMBOL_LSB),
            solRegexEngine_characters()
            ),
        solPattern_literal_new(SOL_REGEX_SYMBOL_RSB)
        );
    return l;
}

SolPattern* solRegexEngine_capture()
{
    SolPattern *c = solPattern_concatenate(
        solPattern_concatenate(
            solPattern_literal_new(SOL_REGEX_SYMBOL_LBR),
            solRegexEngine_regex()
            ),
        solPattern_literal_new(SOL_REGEX_SYMBOL_RBR)
        );
}

SolPattern* solRegexEngine_number()
{
    return NULL;
}

SolPattern* solRegexEngine_range()
{
    SolPattern *c = solPattern_concatenate(
        solPattern_concatenate(
            solPattern_concatenate(
                solPattern_literal_new(SOL_REGEX_SYMBOL_LBRACE),
                solRegexEngine_number()
                ),
            solPattern_choose(
                solPattern_empty_new(),
                solPattern_concatenate(
                    solPattern_literal_new(SOL_REGEX_SYMBOL_COMMA),
                    solRegexEngine_number()
                    )
                )
            ),
        solPattern_literal_new(SOL_REGEX_SYMBOL_RBRACE)
        );
    return c;
}

solPattern* solRegexEngine_count()
{
    reutrn solPattern_choose(
        solPattern_choose(
            solPattern_literal_new(SOL_REGEX_SYMBOL_STAR),
            solPattern_literal_new(SOL_REGEX_SYMBOL_PLUS)),
        solPattern_choose(
            solPattern_literal_new(SOL_REGEX_SYMBOL_QM),
            solRegexEngine_range())
        );
}

SolPattern* solRegexEngine_regex()
{
    return NULL;
}

SolPattern* solRegexEngine_conv_pattern(SolRegexEngine *re, void *)
{
}

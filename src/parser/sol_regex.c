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

SolPattern* solRegexEngine_regex_pattern()
{
    return NULL;
}

SolPattern* solRegexEngine_letter_pattern()
{
    return NULL;
}

SolPattern* solRegexEngine_number_pattern()
{
    return NULL;
}

SolPattern* solRegexEngine_character_pattern()
{
    return solPattern_choose(
        solRegexEngine_letter_pattern(),
        solRegexEngine_number_pattern()
        );
}

SolPattern* solRegexEngine_list_pattern()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solPattern_literal_new(SOL_REGEX_SYMBOL_LSB),
            solPattern_repeat(
                solPattern_choose(
                    solPattern_choose(
                        solRegexEngine_character_pattern(),
                        solRegexEngine_numbers_abbr_pattern()
                        ),
                    solRegexEngine_letters_abbr_pattern()
                    )
                )
            ),
        solPattern_literal_new(SOL_REGEX_SYMBOL_RSB)
        );
}

SolPattern* solRegexEngine_capture_pattern()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solPattern_literal_new(SOL_REGEX_SYMBOL_LBR),
            solRegexEngine_regex_pattern()
            ),
        solPattern_literal_new(SOL_REGEX_SYMBOL_RBR)
        );
}

SolPattern* solRegexEngine_range_pattern()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solPattern_concatenate(
                solPattern_literal_new(SOL_REGEX_SYMBOL_LBRACE),
                solRegexEngine_number_pattern()
                ),
            solPattern_choose(
                solPattern_empty_new(),
                solPattern_concatenate(
                    solPattern_literal_new(SOL_REGEX_SYMBOL_COMMA),
                    solRegexEngine_number_pattern()
                    )
                )
            ),
        solPattern_literal_new(SOL_REGEX_SYMBOL_RBRACE)
        );
}

solPattern* solRegexEngine_count_pattern()
{
    reutrn solPattern_choose(
        solPattern_choose(
            solPattern_literal_new(SOL_REGEX_SYMBOL_STAR),
            solPattern_literal_new(SOL_REGEX_SYMBOL_PLUS)
            ),
        solPattern_choose(
            solPattern_literal_new(SOL_REGEX_SYMBOL_QM),
            solRegexEngine_range_pattern()
            )
        );
}

SolPattern* solRegexEngine_numbers_abbr_pattern()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solRegexEngine_number_pattern(),
            solPattern_literal_new(SOL_REGEX_SYMBOL_MIUS)
            ),
        solRegexEngine_number_pattern()
        );
}

SolPattern* solRegexEngine_letters_abbr_pattern()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solRegexEngine_letter_pattern(),
            solPattern_literal_new(SOL_REGEX_SYMBOL_MIUS)
            ),
        solRegexEngine_letter_pattern()
        );
}

SolPattern* solRegexEngine_conv_pattern(SolRegexEngine *re, void *)
{
}

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
    return solPattern_choose(
        solPattern_choose(
            solPattern_empty_new()
            ),
        solPattern_choose(
            solPattern_choose(
                solRegexEngine_character_pattern(),
                solRegexEngine_count_pattern()
                ),
            solPattern_choose(
                solRegexEngine_capture_pattern(),
                solRegexEngine_list_pattern()
                )
            )
        );
}

SolPattern* solRegexEngine_letter_pattern()
{
    return solPattern_literal_new(SolRegexLiteralType_Letter);
}

SolPattern* solRegexEngine_number_pattern()
{
    return solPattern_literal_new(SolRegexLiteralType_Number);
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
            solPattern_literal_new(SolRegexSymbol_List_ls),
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
        solPattern_literal_new(SolRegexSymbol_List_rs)
        );
}

SolPattern* solRegexEngine_capture_pattern()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solPattern_literal_new(SolRegexSymbol_Capture_ls),
            solRegexEngine_regex_pattern()
            ),
        solPattern_literal_new(SolRegexSymbol_Capture_rs)
        );
}

SolPattern* solRegexEngine_range_pattern()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solPattern_concatenate(
                solPattern_literal_new(SolRegexSymbol_Range_ls),
                solRegexEngine_number_pattern()
                ),
            solPattern_choose(
                solPattern_empty_new(),
                solPattern_concatenate(
                    solPattern_literal_new(SolRegexSymbol_Range_sep),
                    solRegexEngine_number_pattern()
                    )
                )
            ),
        solPattern_literal_new(SolRegexSymbol_Range_rs)
        );
}

SolPattern* solRegexEngine_count_pattern()
{
    reutrn solPattern_choose(
        solPattern_choose(
            solPattern_literal_new(SolRegexSymbol_Repeat_or_empty),
            solPattern_literal_new(SolRegexSymbol_Repeat_or_once)
            ),
        solPattern_choose(
            solPattern_literal_new(SolRegexSymbol_Empty_or_once),
            solRegexEngine_range_pattern()
            )
        );
}

SolPattern* solRegexEngine_numbers_abbr_pattern()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solRegexEngine_number_pattern(),
            solPattern_literal_new(SolRegexSymbol_abbr_sep)
            ),
        solRegexEngine_number_pattern()
        );
}

SolPattern* solRegexEngine_letters_abbr_pattern()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solRegexEngine_letter_pattern(),
            solPattern_literal_new(SolRegexSymbol_abbr_sep)
            ),
        solRegexEngine_letter_pattern()
        );
}

SolPattern* solRegexEngine_conv_pattern(SolRegexEngine *re, void *)
{
}

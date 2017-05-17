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

SolPattern* solRegexEngine_pattern_regex()
{
    return solPattern_choose(
        solPattern_choose(
            solPattern_empty_new()
            ),
        solPattern_choose(
            solPattern_choose(
                solRegexEngine_pattern_character(),
                solRegexEngine_pattern_count()
                ),
            solPattern_choose(
                solPattern_choose(
                    solRegexEngine_pattern_capture(),
                    solRegexEngine_pattern_list()
                    )
                solRegexEngine_pattern_not_greed()
                ),
            )
        );
}

SolPattern* solRegexEngine_pattern_letter()
{
    return solPattern_literal_new(SolRegexLiteralType_Letter);
}

SolPattern* solRegexEngine_pattern_number()
{
    return solPattern_literal_new(SolRegexLiteralType_Number);
}

SolPattern* solRegexEngine_pattern_element()
{
    return solPattern_choose(
        solRegexEngine_pattern_character(),
        solRegexEngine_group_pattern()
        );
}

SolPattern* solRegexEngine_pattern_X_0_()
{
    return solPattern_concatenate(
        solRegexEngine_pattern_element(),
        solPattern_literal_new(SolRegexSymbol_X_0_)
        );
}

SolPattern* solRegexEngine_pattern_X_1_()
{
    return solPattern_concatenate(
        solRegexEngine_pattern_element(),
        solPattern_literal_new(SolRegexSymbol_X_1_)
        );
}

SolPattern* solRegexEngine_pattern_X_0_1()
{
    return solPattern_concatenate(
        solRegexEngine_pattern_element(),
        solPattern_literal_new(SolRegexSymbol_X_0_1)
        );
}

SolPattern* solRegexEngine_pattern_character()
{
    return solPattern_choose(
        solRegexEngine_pattern_letter(),
        solRegexEngine_pattern_number()
        );
}

SolPattern* solRegexEngine_pattern_list()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solPattern_literal_new(SolRegexSymbol_List_ls),
            solPattern_repeat(
                solPattern_choose(
                    solPattern_choose(
                        solRegexEngine_pattern_character(),
                        solRegexEngine_numbers_Abbr_pattern()
                        ),
                    solRegexEngine_letters_Abbr_pattern()
                    )
                )
            ),
        solPattern_literal_new(SolRegexSymbol_List_rs)
        );
}

SolPattern* solRegexEngine_pattern_capture()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solPattern_literal_new(SolRegexSymbol_Capture_ls),
            solRegexEngine_pattern_regex()
            ),
        solPattern_literal_new(SolRegexSymbol_Capture_rs)
        );
}

SolPattern* solRegexEngine_group_pattern()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solPattern_literal_new(SolRegexSymbol_Group_ls),
            solRegexEngine_pattern_regex()
            ),
        solPattern_literal_new(SolRegexSymbol_Group_rs)
        );
}

SolPattern* solRegexEngine_pattern_range()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solPattern_concatenate(
                solPattern_literal_new(SolRegexSymbol_Range_ls),
                solRegexEngine_pattern_number()
                ),
            solPattern_choose(
                solPattern_empty_new(),
                solPattern_concatenate(
                    solPattern_literal_new(SolRegexSymbol_Range_sep),
                    solRegexEngine_pattern_number()
                    )
                )
            ),
        solPattern_literal_new(SolRegexSymbol_Range_rs)
        );
}

SolPattern* solRegexEngine_pattern_count()
{
    reutrn solPattern_choose(
        solPattern_choose(
            solRegexEngine_pattern_X_0_(),
            solRegexEngine_pattern_X_1_()
            ),
        solPattern_choose(
            solRegexEngine_pattern_X_0_1(),
            solRegexEngine_pattern_range()
            )
        );
}

SolPattern* solRegexEngine_pattern_numbers_abbr()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solRegexEngine_pattern_number(),
            solPattern_literal_new(SolRegexSymbol_Abbr_sep)
            ),
        solRegexEngine_pattern_number()
        );
}

SolPattern* solRegexEngine_pattern_letters_abbr()
{
    return solPattern_concatenate(
        solPattern_concatenate(
            solRegexEngine_pattern_letter(),
            solPattern_literal_new(SolRegexSymbol_Abbr_sep)
            ),
        solRegexEngine_pattern_letter()
        );
}

SolPattern* solRegexEngine_pattern_not_greed()
{
    return solPattern_concatenate(
        solRegexEngine_pattern_count(),
        solPattern_literal_new(SolRegexSymbol_Not_greed)
        );
}

SolPattern* solRegexEngine_conv_pattern(SolRegexEngine *re, void *)
{
}

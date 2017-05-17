#ifndef _SOL_REGEX_SYMBOL_H_
#define _SOL_REGEX_SYMBOL_H_ 1

enum SolRegexSymbol {
    SolRegexSymbol_Number = 1,
    SolRegexSymbol_Letter,
    SolRegexSymbol_X_0_1, // ? once or empty
    SolRegexSymbol_X_0_, // * repeat or empty
    SolRegexSymbol_X_1_, // + repeat
    SolRegexSymbol_Capture_ls, // capture left sign
    SolRegexSymbol_Capture_rs, // capture right sign
    SolRegexSymbol_List_ls, // list left sign
    SolRegexSymbol_List_rs, // list right sign
    SolRegexSymbol_Range_ls, // range left sign
    SolRegexSymbol_Range_sep, // range mid separate
    SolRegexSymbol_Range_rs, // range right sign
    SolRegexSymbol_Group_ls,
    SolRegexSymbol_Group_rs,
    SolRegexSymbol_Abbr_sep, // range mid separate
    SolRegexSymbol_Not_greed,
}

#endif

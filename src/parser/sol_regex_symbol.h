#ifndef _SOL_REGEX_SYMBOL_H_
#define _SOL_REGEX_SYMBOL_H_ 1

enum SolRegexSymbol {
    SolRegexSymbol_None = 0,
    SolRegexSymbol_Number,
    SolRegexSymbol_Letter,
    SolRegexSymbol_Sign,
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
};

typedef struct _SolRegexReader {
    size_t o;
    size_t size;
    void *s;
} SolRegexReader;

#deifne solRegexReader_str(r) (r)->s
#define solRegexReader_str_size(r) (r)->size
#define solRegexReader_current(r) (r)->s + (r)->o
#define solRegexReader_next(r) (r)->s + (++((r)->o))

#endif

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

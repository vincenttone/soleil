#include <stdio.h>
#include "sol_slr.h"

int main()
{
    SolSLRParser *p = solSLRParser_new();
    printf("prepare return %d\n", solSLRParser_prepare(p));
    solSLRParser_free(p);
    return 0;
}

#include <stdio.h>
#include "sol_slr.h"

int main()
{
    SolSLRParser *p = solSLRParser_new();
    solSLRParser_free(p);
    return 0;
}

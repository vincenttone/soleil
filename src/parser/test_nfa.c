#include <stdio.h>
#include <string.h>
#include "sol_nfa.h"

int main()
{
	solNfa *nfa = solNfa_new();
	solNfa_free(nfa);
	return 0;
}

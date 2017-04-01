#include <stdio.h>
#include <string.h>
#include "sol_nfa.h"

int main()
{
	SolNfa *nfa = solNfa_new();
	solNfa_free(nfa);
	return 0;
}

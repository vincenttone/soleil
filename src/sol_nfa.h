#ifndef _SOL_NFA_H_
#define _SOL_NFA_H_ 1

#include "sol_common.h"

typedef struct NfaRules {
	SolList *rules;
} NfaRules;

typedef struct NfaRulesBook {
	SolList *rules;
} NfaRulesBook;

#endif

#ifndef _SOL_PDA_H_
#define _SOL_PDA_H_ 1

#include "sol_common.h"
#include "sol_table_fixed.h"
#include "sol_list.h"
#include "sol_stack.h"

#define SolPdaFieldFlag_push   0x1
#define SolPdaFieldFlag_pop    0x2

#define SOL_PDA_MAX_LOOP_COUNT 2048

typedef struct _SolPdaState {
	size_t state;
	SolList *free_moves;
} SolPdaState;

typedef struct _SolPdaSymbol {
	void *symbol;
	size_t c;
} SolPdaSymbol;
	
typedef struct _SolPdaField {
	SolPdaState *state;
	int flag;
} SolPdaField;

typedef struct _SolPda {
	SolPdaState *cs; // current state
	SolPdaState *as; // accepting state
	SolStack *stk;
	SolTableFixed *rules;
	SolList *fields;
	SolList *states; // states
	size_t g2; // symbol count generater
	size_t lc; // loop counter
} SolPda;

SolPda* solPda_new(size_t state_count, size_t symbol_count);
void solPda_free(SolPda*);
int solPda_read(SolPda*, SolPdaSymbol*);
int solPda_is_accepting(SolPda*);
int solPda_check_state_accepting(SolPda*, SolPdaState*);

void solPda_init(SolPda*, SolPdaState*, SolPdaState*);
int solPda_add_rule(SolPda*, SolPdaState*, SolPdaSymbol*, SolPdaState*, int);

SolPdaSymbol* solPda_register_symbol(SolPda*, void*);

SolPdaState* solPda_generate_state(SolPda*);
void solPdaState_free(SolPdaState*);

SolPdaField* solPdaField_new(SolPdaState*, int);
void solPdaField_free(SolPdaField*);

#endif

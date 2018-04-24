#ifndef _SOL_PDA_H_
#define _SOL_PDA_H_ 1

#include <stdarg.h>
#include "sol_common.h"
#include "sol_table_fixed.h"
#include "sol_list.h"
#include "sol_stack.h"
#include "sol_hash.h"

#define SolPdaFieldFlag_stack_push   0x1
#define SolPdaFieldFlag_stack_pop    0x2
#define SolPdaFieldFlag_stack_empty  0x4

#define SOL_PDA_MAX_LOOP_COUNT 2048

#define SOL_PDA_SYMBOL_FLAG_GROUP   0x1
#define SOL_PDA_SYMBOL_FLAG_VIRTUAL 0x2

typedef struct _SolPdaState {
	size_t state;
	SolList *free_moves;
} SolPdaState;

typedef struct _SolPdaSymbol {
	void *symbol;
	size_t c;
	int flag;
} SolPdaSymbol;

typedef struct _SolPda {
	SolPdaState *cs; // current state
	SolPdaState *as; // accepting state
	SolStack *stk;
	SolTableFixed *rules;
	SolList *fields;
	SolList *states; // states
	SolList *symbols; // symbols
	SolHash *symbol_map;
	int (*state_change_cb)(struct _SolPda*, SolPdaSymbol*, SolPdaState*, SolPdaState*, int, void*);
	size_t lc; // loop counter
	int act; // pre action
} SolPda;

typedef struct _SolPdaField {
	SolPdaState *state;
	SolPdaSymbol *symbol; // pop or push
	int (*_callback)(void*);
	int flag;
} SolPdaField;

SolPda* solPda_new();
void solPda_free(SolPda*);
int solPda_read(SolPda *, void*, void*);
int solPda_read_symbol(SolPda*, SolPdaSymbol*, void*);
int solPda_is_accepting(SolPda*);
int solPda_check_state_accepting(SolPda*, SolPdaState*);
SolPdaField* solPda_free_moves_find(SolPda *pda, SolPdaState *s, SolPdaSymbol *sbl);
int solPda_gen_rules_table(SolPda*);

void solPda_init(SolPda*, SolPdaState*, SolPdaState*);
int solPda_add_rule(SolPda*,
					SolPdaState*,
					SolPdaSymbol*,
					SolPdaState*,
					SolPdaSymbol*,
					int,
					int (*_callback)(void*)
					);

SolPdaSymbol* solPda_register_symbol(SolPda*, void*);
SolPdaSymbol* solPda_register_symbol_group(SolPda*, size_t, ...);
int solPda_append_symbol_to_group(SolPda*, void*, SolPdaSymbol*);
void solPdaSymbol_free(SolPdaSymbol*);

SolPdaState* solPda_generate_state(SolPda*);
void solPdaState_free(SolPdaState*);

SolPdaField* solPdaField_new(SolPdaState*, SolPdaSymbol*, int, int (*_callback)(void*));
void solPdaField_free(SolPdaField*);

#endif

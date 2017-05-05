#ifndef _SOL_PATTERN_H_
#define _SOL_PATTERN_H_ 1

#include <string.h>
#include "sol_common.h"
#include "sol_utils.h"
#include "sol_stack.h"
#include "sol_dfa.h"

#define SolPatternState unsigned int
#define SolPatternCharacter char
#define _SOL_PATTERN_STATE_MAX 65535

typedef struct _SolPattern {
	SolDfa *dfa;
	SolStack *s; // state stack
} SolPattern;

typedef struct _SolPatternStateGen {
	SolPatternState i;
	SolPatternState l[_SOL_PATTERN_STATE_MAX];
} SolPatternStateGen;

#define solPattern_dfa(p) p->dfa
#define solPattern_state_stack(p) p->s
#define solPattern_character_at_offset(s, d, o) strncpy(s, (d+o), sizeof(char))

SolPattern* solPattern_new();
void solPattern_free(SolPattern*);
int solPattern_push_state(SolPattern*, SolPatternState*);
SolPatternState* solPattern_pop_state(SolPattern*);

SolPatternStateGen* solPatternStateGen_new();
void solPatternStateGen_free(SolPatternStateGen*);
SolPatternState* solPatternGen_gen_state(SolPatternStateGen*);

int solPattern_is_match(SolPattern*, SolPatternCharacter*, size_t size);

SolPattern* solPattern_empty_new(SolPatternStateGen*);
SolPattern* solPattern_literal_new(SolPatternStateGen*, SolPatternCharacter*);
SolPattern* solPattern_repeat(SolPattern *);
SolPattern* solPattern_concatenate(SolPattern*, SolPattern*);
SolPattern* solPattern_choose(SolPattern*, SolPattern*);

int _solPattern_char_equal(void *c1, void *c2);
int _solPattern_state_equal(void *s1, void *s2);
void _solPattern_debug_relations(SolPattern *p);

#define _solPattern_debug_dfa_relations(p) 	_solDfa_debug_relations(solPattern_dfa(p))

#endif

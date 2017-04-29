#ifndef _SOL_PATTERN_H_
#define _SOL_PATTERN_H_ 1

#include "sol_nfa.h"

#define SolPatternState int
#define _SOL_PATTERN_STATE_MAX 65535

typedef struct _SolPattern {
	SolNfa *nfa;
} SolPattern;

typedef struct _SolPatternStateGen {
	SolPatternState i;
	SolPatternState l[_SOL_PATTERN_STATE_MAX];
} SolPatternStateGen;

SolPattern* solPattern_new();
void solPattern_free(SolPattern*);

int solPattern_reset(SolPattern *p);
int solPattern_match(SolPattern *p, char *s, size_t size);

SolPatternStateGen* solPatternStateGen_new();
void solPatternStateGen_free(SolPatternStateGen*);

SolPatternState* solPattern_gen_state(SolPatternStateGen*);
inline SolNfa* solPattern_gen_nfa();

SolPattern* solPattern_empty_new(SolPatternStateGen*);
SolPattern* solPattern_literal_new(SolPatternStateGen*, void*);

SolPattern* solPattern_concatenate(SolPattern*, SolPattern*);
SolPattern* solPattern_choose(SolPattern*, SolPattern*);
SolPattern* solPattern_repeat(SolPattern*);

int _solPattern_state_equal(void *k1, void *k2);
int _solPattern_char_equal(void *c1, void *c2);

void _solPattern_debug_relations(SolPattern *p);

#endif

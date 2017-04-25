#ifndef _SOL_PATTERN_H_
#define _SOL_PATTERN_H_ 1

#include "sol_nfa.h"

typedef struct _SolPattern {
	SolNfa *nfa;
} SolPattern;

struct _SolPatternStateGen {
	int i;
};

SolPattern* solPattern_new();
void solPattern_free(SolPattern*);

int solPattern_match(SolPattern*, SolVal*);

struct _SolPatternStateGen* solPatternStateGen_new();
SolVal* solPattern_gen_state(struct _SolPatternStateGen*);
inline SolNfa* solPattern_gen_nfa();

SolPattern* solPattern_empty_new(struct _SolPatternStateGen*);
SolPattern* solPattern_literal_new(struct _SolPatternStateGen*, void*);
int solPattern_concatenate(SolPattern*, SolPattern*);
int solPattern_choose(SolPattern*, SolPattern*);
int solPattern_repeat(SolPattern*);

int _solPattern_val_equal(void *k1, void *k2);
int _solPattern_char_equal(void *c1, void *c2);

#endif

#ifndef _SOL_PATTERN_H_
#define _SOL_PATTERN_H_ 1

#include "sol_list.h"
#include "sol_nfa.h"

typedef struct _SolPattern {
	int c; // state counter
	SolList *l; // nfa rule list
} SolPattern;

SolPattern* solPattern_new();
void solPattern_free(SolPattern*);
SolVal* solPattern_gen_state(SolPattern*);
SolNfa* solPattern_gen_nfa();
#define solPattern_add_nfa(p, n) solList_add(p->l, n)

SolNfa* solPattern_empty_new(SolPattern*);
SolNfa* solPattern_literal_new(SolPattern*, SolVal*);
SolNfa* solPattern_concatenate_new(SolPattern*, SolVal*, SolVal*);
SolNfa* solPattern_choose_new(SolPattern*, SolVal*, SolVal*);
SolNfa* solPattern_repeat_new(SolPattern*, SolVal*);

int _solPattern_val_equal(void *k1, void *k2);

#end

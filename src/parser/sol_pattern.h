#ifndef _SOL_PATTERN_H_
#define _SOL_PATTERN_H_ 1

#include <string.h>
#include "sol_common.h"
#include "sol_utils.h"
#include "sol_dfa.h"
#include "sol_list.h"

#define SolPatternState unsigned int

typedef struct _SolPattern {
    SolDfa *dfa;
    SolList *cl; // capture list
    size_t (*r)(void*); // read literal
} SolPattern;

typedef struct _SolPatternStateGen {
    SolPatternState i;
    SolList *l;
} SolPatternStateGen;

enum SolPatternCaptureMarkFlag {
    SolPatternCaptureMarkFlag_None = 0x0,
    SolPatternCaptureMarkFlag_Greed = 0x2,
    SolPatternCaptureMarkFlag_Expect_end = 0x4, // 0: expect begin 1: expect end
    SolPatternCaptureMarkFlag_Matched = 0x8,
};
#define _solPatternCaptureMark_flush_expect_end(f) (f) & 0xfb
#define _solPatternCaptureMark_flush_matched(f) (f) & 0xf7

enum SolPatternDfaStateFlag {
    SolPatternDfaStateFlag_None = 0x0,
    SolPatternDfaStateFlag_Begin = 0x2, // begin flag of capture
    SolPatternDfaStateFlag_End = 0x4, // end flag of capture
    SolPatternDfaStateFlag_Is_initial = 0x8, // begining of string
    SolPatternDfaStateFlag_Is_final = 0x10, // ending of string
    SolPatternDfaStateFlag_Is_cm = 0x20, // is capture mark
};

typedef struct _SolPatternCaptureMark {
    size_t is; // starting index
    size_t ie; // end index
    int flag; // flag
    void *tag;
} SolPatternCaptureMark;

enum _SolPatternReadStrStatus {
    _SolPatternReadStrStatus_Normal = 1,
    _SolPatternReadStrStatus_Begining = 2,
    _SolPatternReadStrStatus_Ending = 3,
};

#define solPattern_dfa(p) (p)->dfa
#define solPattern_capture_list(p) (p)->cl
#define solPattern_reading_literal_func(p) (p)->r

#define solPattern_set_capture_list(p, l) (p)->cl = l
#define solPattern_set_reading_literal_func(p, f) (p)->r = f

#define solPattern_read_literal(p, s) (*p->r)(s)

#define solPatternCaptureMark_starting_index(cm) (cm)->is
#define solPatternCaptureMark_end_index(cm) (cm)->ie
#define solPatternCaptureMark_tag(cm) (cm)->tag
#define solPatternCaptureMark_flag(cm) (cm)->flag

#define solPatternCaptureMark_set_tag(cm, t) (cm)->tag = t
#define solPatternCaptureMark_set_starting_index(cm, i) (cm)->is = i
#define solPatternCaptureMark_set_end_index(cm, l) (cm)->ie = l
#define solPatternCaptureMark_set_flag(cm, f) (cm)->flag = f

#define solPattern_state_marked_inital(dsm) (solDfaStateMark_flag(dsm) & SolPatternDfaStateFlag_Is_initial)
#define solPattern_state_marked_final(dsm) (solDfaStateMark_flag(dsm) & SolPatternDfaStateFlag_Is_final)

SolPattern* solPattern_new();
void solPattern_free(SolPattern*);

SolPatternStateGen* solPatternStateGen_new();
void solPatternStateGen_free(SolPatternStateGen*);
SolPatternState* solPatternGen_gen_state(SolPatternStateGen*);

int solPattern_check_matching(SolPattern*);
void solPattern_reset(SolPattern*);
int solPattern_read_character(SolPattern*, void*);
int solPattern_match(SolPattern*, void*, size_t);

SolPattern* solPattern_empty_new(SolPatternStateGen*);
SolPattern* solPattern_literal_new(SolPatternStateGen*, void*);
SolPattern* solPattern_concatenate_new(SolPatternStateGen*, SolList*);
SolPattern* solPattern_choose_new(SolPatternStateGen*, SolList*);
SolPattern* solPattern_repeat_new(SolPatternStateGen*, void*);
SolPattern* solPattern_repeat(SolPattern *);
SolPattern* solPattern_concatenate(SolPattern*, SolPattern*);
SolPattern* solPattern_choose(SolPattern*, SolPattern*);
SolPattern* solPattern_capture(SolPattern*, enum SolPatternCaptureMarkFlag, void*);
SolPattern* solPattern_begin_with(SolPattern*);
SolPattern* solPattern_end_with(SolPattern*);

int _solPattern_char_equal(void *c1, void *c2);
int _solPattern_state_equal(void *s1, void *s2);
void _solPattern_debug_relations(SolPattern *p);

#endif

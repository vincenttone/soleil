#include <stdio.h>
#include <string.h>
#include "sol_pattern.h"

SolPattern* solPattern_new()
{
    SolPattern *p = sol_calloc(1, sizeof(SolPattern));
    if (p == NULL) {
        return NULL;
    }
    if (solPattern_dfa(p) == NULL) {
        p->dfa = solDfa_new(&sol_i_hash_func1, &sol_i_hash_func2, &_solPattern_state_equal,
                            &sol_c_hash_func1, &sol_c_hash_func2, &_solPattern_char_equal);
    }
    if (solPattern_dfa(p) == NULL) {
        solPattern_free(p);
        return NULL;
    }
    return p;
}

void solPattern_free(SolPattern *p)
{
    if (p->dfa) {
        solDfa_free(p->dfa);
    }
    if (solPattern_state_stack(p)) {
        solStack_free(solPattern_state_stack(p));
    }
    if (solPattern_capture_list(p)) {
        solList_free(solPattern_capture_list(p));
    }
    if (p) {
        sol_free(p);
    }
}

int solPattern_push_state(SolPattern *p, SolPatternState *s)
{
    if (solPattern_state_stack(p) == NULL) {
        p->s = solStack_new();
    }
    if (solPattern_state_stack(p) == NULL) {
        return 1;
    }
    if (solStack_push(solPattern_state_stack(p), s)) {
        return 0;
    }
    return 2;
}

SolPatternState* solPattern_pop_state(SolPattern *p)
{
    if (solPattern_state_stack(p) == NULL) {
        return NULL;
    }
    SolPatternState *s = (SolPatternState*)(solStack_pop(solPattern_state_stack(p)));
    return s;
}

SolPatternStateGen* solPatternStateGen_new()
{
    SolPatternStateGen *g = sol_calloc(1, sizeof(SolPatternStateGen));
    g->i = 0;
    return g;
}

void solPatternStateGen_free(SolPatternStateGen *g)
{
    sol_free(g);
}

SolPatternState* solPatternGen_gen_state(SolPatternStateGen *g)
{
    g->i++;
    g->l[g->i] = g->i;
    return &(g->l[g->i]);
}

void solPattern_reset(SolPattern *p)
{
    solPattern_reset_capture_mark(p);
    solDfa_reset_current_state(solPattern_dfa(p));
}

int solPattern_check_matching(SolPattern *p)
{
    if (p == NULL) {
        return -1;
    }
    if (solPattern_dfa(p) == NULL) {
        return -2;
    }
    if (solDfa_is_accepting(solPattern_dfa(p)) == 0) {
        return 0;
    }
    return 1;
}

int solPattern_read_character(SolPattern *p, void *c)
{
    if (p == NULL) {
        return -1;
    }
    if (solPattern_dfa(p) == NULL) {
        return -2;
    }
    int r = solDfa_read_character(solPattern_dfa(p), c);
    if (r == 0) {
        return 0;
    } else if (r == 1) {
        return 1;
    } else {
        return -3;
    }
}

SolPattern* solPattern_empty_new(SolPatternStateGen *g)
{
    SolPattern *p = solPattern_new();
    SolPatternState *s = solPatternGen_gen_state(g);
    if (solDfa_set_starting_state(solPattern_dfa(p), s) != 0) {
        solPattern_free(p);
        return NULL;
    }
    if (solDfa_add_accepting_state(solPattern_dfa(p), s) != 0) {
        solPattern_free(p);
        return NULL;
    }
    return p;
}

SolPattern* solPattern_literal_new(SolPatternStateGen *g, void *c)
{
    SolPattern *p = solPattern_new();
    SolPatternState *s1 = solPatternGen_gen_state(g);
    SolPatternState *s2 = solPatternGen_gen_state(g);
    if (solDfa_set_starting_state(solPattern_dfa(p), s1) != 0) {
        solPattern_free(p);
        return NULL;
    }
    if (solDfa_add_accepting_state(solPattern_dfa(p), s2) != 0) {
        solPattern_free(p);
        return NULL;
    }
    solDfa_add_rule(solPattern_dfa(p), s1, s2, c);
    return p;
}

SolPattern* solPattern_repeat(SolPattern *p)
{
    if (p == NULL) {
        return NULL;
    }
    void *s;
    solDfa_accepting_states_rewind(solPattern_dfa(p));
    while ((s = (solDfa_accepting_states_get_one(solPattern_dfa(p))))) {
        if (solDfa_state_merge(solPattern_dfa(p),
                               solPattern_dfa(p),
                               solDfa_starting_state(solPattern_dfa(p)),
                               s
                               ) != 0
            ) {
            return NULL;
        }
    }
    solDfa_wipe_accepting_states(solPattern_dfa(p));
    if (solDfa_add_accepting_state(solPattern_dfa(p), solDfa_starting_state(solPattern_dfa(p))) != 0) {
        return NULL;
    }
    return p;
}

SolPattern* solPattern_concatenate(SolPattern *p1, SolPattern *p2)
{
    if (p1 == NULL || p2 == NULL) {
        return NULL;
    }
    SolPattern ptmp;
    ptmp = *p2;
    *p2 = *p1;
    *p1 = ptmp;
    solDfa_merge_all_states(solPattern_dfa(p1), solPattern_dfa(p2));
    solDfa_wipe_all_states(solPattern_dfa(p2));
    solDfa_free_all_states(solPattern_dfa(p2));
    solDfa_set_all_states(solPattern_dfa(p2), solDfa_all_states(solPattern_dfa(p1)));
    void *s;
    solDfa_accepting_states_rewind(solPattern_dfa(p2));
    while ((s = (solDfa_accepting_states_get_one(solPattern_dfa(p2))))) {
        if (solDfa_state_merge(solPattern_dfa(p1),
                               solPattern_dfa(p2),
                               solDfa_starting_state(solPattern_dfa(p1)),
                               s
                               ) != 0
            ) {
            return NULL;
        }
    }
    solDfa_set_starting_state(solPattern_dfa(p1), solDfa_starting_state(solPattern_dfa(p2)));
    solPattern_set_reading_literal_func(p1, solPattern_reading_literal_func(p2));
    if (solPattern_capture_list(p1)) {
        solList_merge(solPattern_capture_list(p1), solPattern_capture_list(p2));
    } else if (solPattern_capture_list(p2)) {
        solPattern_set_capture_list(p1, solPattern_capture_list(p2));
    }
    solPattern_set_capture_list(p2, NULL);
    solDfa_set_all_states(solPattern_dfa(p2), NULL);
    solPattern_free(p2);
    return p1;
}

SolPattern* solPattern_choose(SolPattern *p1, SolPattern *p2)
{
    if (p1 == NULL || p2 == NULL) {
        return NULL;
    }
    solDfa_merge_all_states(solPattern_dfa(p1), solPattern_dfa(p2));
    solDfa_wipe_all_states(solPattern_dfa(p2));
    solDfa_free_all_states(solPattern_dfa(p2));
    solDfa_set_all_states(solPattern_dfa(p2), solDfa_all_states(solPattern_dfa(p1)));
    if (solDfa_state_merge(solPattern_dfa(p1),
                           solPattern_dfa(p2),
                           solDfa_starting_state(solPattern_dfa(p1)),
                           solDfa_starting_state(solPattern_dfa(p2))
                           ) != 0
        ) {
        return NULL;
    }
    solDfa_merge_accepting_states(solPattern_dfa(p1), solPattern_dfa(p2));
    if (solPattern_capture_list(p1)) {
        solList_merge(solPattern_capture_list(p1), solPattern_capture_list(p2));
    } else if (solPattern_capture_list(p2)) {
        solPattern_set_capture_list(p1, solPattern_capture_list(p2));
    }
    solPattern_set_capture_list(p2, NULL);
    solDfa_set_all_states(solPattern_dfa(p2), NULL);
    solPattern_free(p2);
    return p1;
}

SolPattern* solPattern_capture(SolPattern *p, enum SolPatternCaptureMarkFlag f, void *t)
{
    if (p == NULL) {
        return NULL;
    }
    if (solPattern_dfa(p) == NULL
        || solDfa_starting_state(solPattern_dfa(p)) == NULL
        || solDfa_accepting_states(solPattern_dfa(p)) == NULL) {
        solPattern_free(p);
        return NULL;
    }
    if (solPattern_capture_list(p) == NULL) {
        solPattern_set_capture_list(p, solList_new());
        if (solPattern_capture_list(p) == NULL) {
            solPattern_free(p);
            return NULL;
        }
        solList_set_free_func(solPattern_capture_list(p), &sol_free);
    }
    SolPatternCaptureMark* cm = sol_calloc(1, sizeof(SolPatternCaptureMark));
    if (cm == NULL) {
        solPattern_free(p);
        return NULL;
    }
    solPatternCaptureMark_set_tag(cm, t);
    solPatternCaptureMark_set_flag(cm, f);
    solList_add_fwd(solPattern_capture_list(p), cm);
    SolDfaState *ds = solDfa_conv_dfa_state(solPattern_dfa(p), solDfa_starting_state(solPattern_dfa(p)));
    SolDfaStateMark *dsm = solDfaState_mark(ds);
    int flag = SolPatternDfaStateFlag_Begin | SolPatternDfaStateFlag_Is_cm;
    if (dsm) {
        flag |= solDfaStateMark_flag(dsm);
    }
    solDfaState_add_mark(ds, cm, flag);
    solSet_rewind(solDfa_accepting_states(solPattern_dfa(p)));
    void *s;
    while ((s = solSet_get(solDfa_accepting_states(solPattern_dfa(p))))) {
        ds = solDfa_conv_dfa_state(solPattern_dfa(p), s);
        dsm = solDfaState_mark(ds);
        solPatternCaptureMark_set_flag(cm, f);
        flag = SolPatternDfaStateFlag_End | SolPatternDfaStateFlag_Is_cm;
        if (dsm) {
            flag |= solDfaStateMark_flag(dsm);
        }
        solDfaState_add_mark(ds, cm, flag);
    }
    return p;
}

SolPattern* solPattern_begin_with(SolPattern *p)
{
    if (p == NULL) {
        return NULL;
    }
    if (solPattern_dfa(p) == NULL
        || solDfa_starting_state(solPattern_dfa(p)) == NULL
        || solDfa_accepting_states(solPattern_dfa(p)) == NULL) {
        solPattern_free(p);
        return NULL;
    }
    SolDfaState *ds = solDfa_conv_dfa_state(solPattern_dfa(p), solDfa_starting_state(solPattern_dfa(p)));
    solDfaState_add_mark(ds, NULL, SolPatternDfaStateFlag_Is_initial);
    return p;
}

SolPattern* solPattern_end_with(SolPattern *p)
{
    if (p == NULL) {
        return NULL;
    }
    if (solPattern_dfa(p) == NULL
        || solDfa_starting_state(solPattern_dfa(p)) == NULL
        || solDfa_accepting_states(solPattern_dfa(p)) == NULL) {
        solPattern_free(p);
        return NULL;
    }
    SolDfaState *ds;
    void *s;
    solSet_rewind(solDfa_accepting_states(solPattern_dfa(p)));
    while ((s = solSet_get(solDfa_accepting_states(solPattern_dfa(p))))) {
        ds = solDfa_conv_dfa_state(solPattern_dfa(p), s);
        solDfaState_add_mark(ds, NULL, SolPatternDfaStateFlag_Is_final);
    }
    return p;
}

int _solPattern_state_equal(void *s1, void *s2)
{
    if ((int*)s1 == (int*)s2) {
        return 0;
    }
    return 1;
}

int _solPattern_char_equal(void *c1, void *c2)
{
    if (*(char*)c1 == *(char*)c2) {
        return 0;
    }
    return 1;
}

int solPattern_match(SolPattern *p, void *str, size_t size)
{
    if (p == NULL) {
        return -1;
    }
    if (solPattern_dfa(p) == NULL) {
        return -2;
    }
    if (solPattern_reading_literal_func(p) == NULL) {
        return -3;
    }
    void *sptr = str;
    void *c;
    int r;
    size_t o;
    size_t fos = 0;
    SolDfaState *cds;
    SolDfaStateMark *m;
    solPattern_reset(p);
    cds = solDfa_conv_dfa_state(solPattern_dfa(p), solDfa_current_state(solPattern_dfa(p)));
    if ((m = solDfaState_mark(cds))) {
        do {
            solPatternCapture_update_mark(m, fos);
        } while ((m = solDfaStateMark_next(m)));
    }
    while (size > fos) {
        o = solPattern_read_literal(p, sptr);
        fos = fos + o;
        c = sol_alloc(o);
        strncpy(c, sptr, o);
#ifdef __SOL_DEBUG__
        printf("READ character: [%s], len: %zu\n", (char*)c, o);
#endif
        r = solDfa_read_character(solPattern_dfa(p), c);
        sol_free(c);
        if (r == 1) {
            if (1) {
                solPattern_reset_unmatched_capture_mark(p);
                solDfa_reset_current_state(solPattern_dfa(p));
                goto next_char;
            }
            return 1;
        } else if (r != 0) {
            return 2;
        }
        cds = solDfa_conv_dfa_state(solPattern_dfa(p), solDfa_current_state(solPattern_dfa(p)));
        if ((m = solDfaState_mark(cds))) {
            do {
                solPatternCapture_update_mark(m, fos);
            } while ((m = solDfaStateMark_next(m)));
        }
    next_char:
        sptr = sptr + o;
    }
    return 0;
}

inline void solPatternCapture_update_mark(SolDfaStateMark *dsm, size_t o)
{
    int dsf = solDfaStateMark_flag(dsm);
    if ((dsf & SolPatternDfaStateFlag_Is_cm) == 0) {
        return;
    }
    SolPatternCaptureMark* m = (SolPatternCaptureMark*)(solDfaStateMark_mark(dsm));
    int f = solPatternCaptureMark_flag(m);
    if (dsf & SolPatternDfaStateFlag_End) {
        if (f & SolPatternCaptureMarkFlag_Expect_end) {
            if (f & SolPatternCaptureMarkFlag_Greed) {
                solPatternCaptureMark_set_len(m, o);
                solPatternCaptureMark_set_flag(m, f | SolPatternCaptureMarkFlag_Matched);
#ifdef __SOL_DEBUG__
                printf("GREED set finish flag to %d, starting: %zu, len: %zu\n",
                       solPatternCaptureMark_flag(m),
                       solPatternCaptureMark_starting_index(m),
                       solPatternCaptureMark_len(m)
                    );
#endif
            } else {
                if (solPatternCaptureMark_len(m) == 0) {
                    solPatternCaptureMark_set_len(m, o);
                    solPatternCaptureMark_set_flag(m, f | SolPatternCaptureMarkFlag_Matched);
#ifdef __SOL_DEBUG__
                    printf("FIRST set finishing flag %d, starting: %zu, len: %zu\n",
                           solPatternCaptureMark_flag(m),
                           solPatternCaptureMark_starting_index(m),
                           solPatternCaptureMark_len(m)
                        );
#endif
                }
            }
        }
    } else if ((dsf & SolPatternDfaStateFlag_Begin)
               && (f & SolPatternCaptureMarkFlag_Expect_end) == 0
        ) {
        solPatternCaptureMark_set_starting_index(m, o);
        solPatternCaptureMark_set_flag(m, f | SolPatternCaptureMarkFlag_Expect_end);
#ifdef __SOL_DEBUG__
        printf("SET starting flag %d, starting %zu!!!!!!\n",
               solPatternCaptureMark_flag(m),
               solPatternCaptureMark_starting_index(m)
            );
#endif
    } else {
#ifdef __SOL_DEBUG__
        printf("SKIP flag %d, starting %zu!!!!!!\n",
               solPatternCaptureMark_flag(m),
               solPatternCaptureMark_starting_index(m)
            );
#endif
    }
}

inline void solPattern_reset_unmatched_capture_mark(SolPattern *p)
{
#ifdef __SOL_DEBUG__
    printf("try reset unmatched capture mark!!!!!!\n");
#endif
    if (solPattern_capture_list(p) == NULL) {
        return;
    }
    SolPatternCaptureMark *m;
    SolListIter *li = solListIter_new(solPattern_capture_list(p), _SolListDirFwd);
    solListIter_rewind(li);
    SolListNode *n;
    int f;
    while ((n = solListIter_next(li))) {
        m = (SolPatternCaptureMark*)(solListNode_val(n));
        if (m == NULL) {
            continue;
        }
        f = solPatternCaptureMark_flag(m);
        if ((f & SolPatternCaptureMarkFlag_Expect_end)
            && (f & SolPatternCaptureMarkFlag_Matched) == 0
            ) {
            solPatternCaptureMark_set_starting_index(m, 0);
            solPatternCaptureMark_set_len(m, 0);
            solPatternCaptureMark_set_flag(m, _solPatternCaptureMark_flush_expect_end(f));
#ifdef __SOL_DEBUG__
            printf("FLUSH flag to %d!!!!!!\n", solPatternCaptureMark_flag(m));
#endif
        }
    }
    solListIter_free(li);
}

inline void solPattern_reset_capture_mark(SolPattern *p)
{
    if (solPattern_capture_list(p) == NULL) {
        return;
    }
    SolPatternCaptureMark *m;
    SolListIter *li = solListIter_new(solPattern_capture_list(p), _SolListDirFwd);
    solListIter_rewind(li);
    SolListNode *n;
    int f;
    while ((n = solListIter_next(li))) {
        m = solListNode_val(n);
        if (m == NULL) {
            continue;
        }
        f = solPatternCaptureMark_flag(m);
#ifdef __SOL_DEBUG__
        printf("RESET flag from %d, wanna %d!!!!!!\n",
               f,
               _solPatternCaptureMark_flush_expect_end(_solPatternCaptureMark_flush_matched(f)));
#endif
        solPatternCaptureMark_set_starting_index(m, 0);
        solPatternCaptureMark_set_len(m, 0);
        solPatternCaptureMark_set_flag(m, _solPatternCaptureMark_flush_expect_end(_solPatternCaptureMark_flush_matched(f)));
#ifdef __SOL_DEBUG__
        printf("RESET flag to %d!!!!!!\n", solPatternCaptureMark_flag(m));
#endif
    }
    solListIter_free(li);
}

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
    solPatternCaptureMark_set_flag(cm, f);
    solPatternCaptureMark_set_tag(cm, t);
    solList_add_fwd(solPattern_capture_list(p), cm);
    SolDfaState *ds = solDfa_conv_dfa_state(solPattern_dfa(p), solDfa_starting_state(solPattern_dfa(p)));
    solDfaState_add_mark(ds, cm);
    solSet_rewind(solDfa_accepting_states(solPattern_dfa(p)));
    void *s;
    while ((s = solSet_get(solDfa_accepting_states(solPattern_dfa(p))))) {
        ds = solDfa_conv_dfa_state(solPattern_dfa(p), s);
        solDfaState_add_mark(ds, cm);
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
    if (solPattern_read_literal_func(p) == NULL) {
        return -3;
    }
    void *sptr = str;
    void *c;
    int r, o;
    size_t os;
    solDfa_reset_current_state(solPattern_dfa(p));
    while ((o = solPattern_read_literal(p, sptr))) {
        os = sizeof(unsigned char) * o;
        c = sol_alloc(os);
        strncpy(c, sptr, os);
        r = solDfa_read_character(solPattern_dfa(p), c);
        sol_free(c);
        if (r == 1) {
            return 1;
        } else if (r != 0) {
            return 2;
        }
        sptr = sptr + os;
    }
    return 0;
}

#include <string.h>
#include <stdio.h>
#include "sol_pattern.h"

int _solPattern_is_match(SolPattern *p, char *s, size_t size)
{
    solPattern_reset(p);
    char st[] = {'0'};
    int i = 0;
    int r;
    while (i < size) {
        strncpy(st, s + i, sizeof(char));
        r = solPattern_read_character(p, st);
        if (r == 1) {
            return 100;
        } else if (r != 0) {
            return 101;
        }
        i++;
    }
    return solPattern_check_matching(p);
}

void _solPattern_debug_dfa_relations(SolPattern *p)
{
    SolDfa *d = solPattern_dfa(p);
    printf("Starting state: (%d):\n", *(int*)solDfa_starting_state(d));
    printf("Current state: (%d):\n", *(int*)solDfa_current_state(d));
    printf("Accepting states:\n");
    void *acs;
    while ((acs = solSet_get(solDfa_accepting_states(d)))) {
        printf("state: %d\n", *(int*)acs);
    }
    printf("All states:\n");
    int *n;
    char *sf;
    SolHashIter *j;
    SolDfaState *s;
    SolHashRecord *r;
    SolDfaState *s1;
    SolDfaStateMark *m;
    SolHashIter *i = solHashIter_new(solDfa_all_states(d));
    solHashIter_rewind(i);
    while ((r = solHashIter_get(i))) {
        n = (int*)r->k;
        s = (SolDfaState*)(r->v);
        if (solDfaState_rules(s)) {
            j = solHashIter_new(solDfaState_rules(s));
            solHashIter_rewind(j);
            while ((r = solHashIter_get(j))) {
                s1 = ((SolDfaState*)r->v);
                printf("rules: (%d) -(%c)-> (%d)\n", *n, *((char*)r->k), *(int*)(s1->s));
            }
            solHashIter_free(j);
        }
        if (solDfaState_mark(s)) {
            m = solDfaState_mark(s);
            do {
                if (solDfaStateMark_flag(m) & SolPatternDfaStateFlag_End) {
                    sf = "END";
                } else {
                    sf = "BEGIN";
                }
                printf("state [%d] has %s mark %s\n",
                       *(int*)(s->s),
                       sf,
                       (char*)(((SolPatternCaptureMark*)(solDfaStateMark_mark(m)))->tag)
                    );
            } while ((m = solDfaStateMark_next(m)));
        }
    }
    solHashIter_free(i);
}

size_t read_char(void* s)
{
    return sizeof(char);
}

void print_match_result(SolPattern *p, char* s)
{
    char *ms = sol_calloc(10, sizeof(char));
    SolListIter *li;
    SolPatternCaptureMark *cm;
    SolListNode *ln;
    if (solPattern_capture_list(p)) {
        li = solListIter_new(solPattern_capture_list(p), _SolListDirFwd);
        solListIter_rewind(li);
        while ((ln = solListIter_next(li))) {
            cm = solListNode_val(ln);
            strncpy(ms, s + solPatternCaptureMark_starting_index(cm), solPatternCaptureMark_len(cm));
            printf("Match mark of [%s] str: [%s], result [%s] (%zu:%zu)\n",
                   (char*)(solPatternCaptureMark_tag(cm)),
                   s,
                   ms,
                   solPatternCaptureMark_starting_index(cm),
                   solPatternCaptureMark_len(cm)
                );
        }
        solListIter_free(li);
    }
    sol_free(ms);
}

int main()
{
    char *s0 = "";
    char *sa = "a";
    char *sb = "b";
    char *sc = "c";
    char *sab = "ab";
    char *saaa = "aaa";
    char *sabc = "abc";
    char *sabcabcabc = "abcabcabc";
    char *sabacabac = "abacabac";
    size_t l0 = strlen(s0);
    size_t l1 = strlen(sa);
    size_t l2 = strlen(sab);
    size_t l3 = strlen(saaa);
    size_t l8 = strlen(sabacabac);
    size_t l9 = strlen(sabcabcabc);
    SolPatternStateGen *g = solPatternStateGen_new();
    SolPattern *empty = solPattern_empty_new(g);
    printf("//\t\"%s\"\tmatch? %d\n", s0, _solPattern_is_match(empty, s0, l0));
    printf("//\t\"%s\"\tmatch? %d\n", sa, _solPattern_is_match(empty, sa, l1));
    printf("//\t\"%s\"\tmatch? %d\n", sab, _solPattern_is_match(empty, sab, l2));
    solPattern_free(empty);
    SolPattern *pa = solPattern_literal_new(g, sa);
    printf("/a/\t\"%s\"\tmatch? %d\n", s0, _solPattern_is_match(pa,     s0, l0));
    printf("/a/\t\"%s\"\tmatch? %d\n", sa, _solPattern_is_match(pa,     sa, l1));
    printf("/a/\t\"%s\"\tmatch? %d\n", sab, _solPattern_is_match(pa, sab, l2));
    solPattern_free(pa);
    SolPattern *prpt = solPattern_repeat(solPattern_literal_new(g, sa));
    printf("/a*/\t\"%s\"\tmatch? %d\n", s0, _solPattern_is_match(prpt,     s0, l0));
    printf("/a*/\t\"%s\"\tmatch? %d\n", sa, _solPattern_is_match(prpt,     sa, l1));
    printf("/a*/\t\"%s\"\tmatch? %d\n", sab, _solPattern_is_match(prpt,     sab, l2));
    printf("/a*/\t\"%s\"\tmatch? %d\n", saaa, _solPattern_is_match(prpt,     saaa, l3));
    solPattern_free(prpt);
    SolPattern *pab = solPattern_concatenate(solPattern_literal_new(g, sa), solPattern_literal_new(g, sb));
    printf("/ab/\t\"%s\"\tmatch? %d\n", s0, _solPattern_is_match(pab,     s0, l0));
    printf("/ab/\t\"%s\"\tmatch? %d\n", sa, _solPattern_is_match(pab,     sa, l1));
    printf("/ab/\t\"%s\"\tmatch? %d\n", sab, _solPattern_is_match(pab,     sab, l2));
    printf("/ab/\t\"%s\"\tmatch? %d\n", saaa, _solPattern_is_match(pab,     saaa, l3));
    printf("/ab/\t\"%s\"\tmatch? %d\n", sabc, _solPattern_is_match(pab,     sabc, l3));
    solPattern_concatenate(pab, solPattern_literal_new(g, sc));
    //_solPattern_debug_dfa_relations(pab);
    printf("/abc/\t\"%s\"\tmatch? %d\n", s0, _solPattern_is_match(pab,     s0, l0));
    printf("/abc/\t\"%s\"\tmatch? %d\n", sa, _solPattern_is_match(pab,     sa, l1));
    printf("/abc/\t\"%s\"\tmatch? %d\n", sab, _solPattern_is_match(pab,     sab, l2));
    printf("/abc/\t\"%s\"\tmatch? %d\n", saaa, _solPattern_is_match(pab,     saaa, l3));
    printf("/abc/\t\"%s\"\tmatch? %d\n", sabc, _solPattern_is_match(pab,     sabc, l3));
    solPattern_repeat(pab);
    printf("/abc*/\t\"%s\"\tmatch? %d\n", s0, _solPattern_is_match(pab,     s0, l0));
    printf("/abc*/\t\"%s\"\tmatch? %d\n", sa, _solPattern_is_match(pab,     sa, l1));
    printf("/abc*/\t\"%s\"\tmatch? %d\n", sab, _solPattern_is_match(pab,     sab, l2));
    printf("/abc*/\t\"%s\"\tmatch? %d\n", saaa, _solPattern_is_match(pab,     saaa, l3));
    printf("/abc*/\t\"%s\"\tmatch? %d\n", sabc, _solPattern_is_match(pab,     sabc, l3));
    printf("/abc*/\t\"%s\"\tmatch? %d\n", sabacabac, _solPattern_is_match(pab,     sabacabac, l8));
    printf("/abc*/\t\"%s\"\tmatch? %d\n", sabcabcabc, _solPattern_is_match(pab,     sabcabcabc, l9));
    solPattern_free(pab);
    SolPattern *pa_b = solPattern_choose(solPattern_literal_new(g, sa), solPattern_literal_new(g, sb));
    printf("/a|b/\t\"%s\"\tmatch? %d\n", s0, _solPattern_is_match(pa_b,     s0, l0));
    printf("/a|b/\t\"%s\"\tmatch? %d\n", sa, _solPattern_is_match(pa_b,     sa, l1));
    printf("/a|b/\t\"%s\"\tmatch? %d\n", sab, _solPattern_is_match(pa_b, sab, l2));
    printf("/a|b/\t\"%s\"\tmatch? %d\n", saaa, _solPattern_is_match(pa_b, saaa, l3));
    solPattern_free(pa_b);
    SolPattern *prabac = solPattern_choose(
        solPattern_concatenate(solPattern_literal_new(g, sa),
                               solPattern_literal_new(g, sb)),
        solPattern_concatenate(solPattern_literal_new(g, sa),
                               solPattern_literal_new(g, sc))
        );
    solPattern_repeat(prabac);
    printf("/((ab)|(ac))*/\t\"%s\"\tmatch? %d\n", s0, _solPattern_is_match(prabac, s0, l0));
    printf("/((ab)|(ac))*/\t\"%s\"\tmatch? %d\n", sa, _solPattern_is_match(prabac, sa, l1));
    printf("/((ab)|(ac))*/\t\"%s\"\tmatch? %d\n", sabacabac, _solPattern_is_match(prabac, sabacabac, l8));
    printf("/((ab)|(ac))*/\t\"%s\"\tmatch? %d\n", sabcabcabc, _solPattern_is_match(prabac, sabcabcabc, l9));
    solPattern_free(prabac);
    SolPattern *pE_a = solPattern_choose(solPattern_empty_new(g), solPattern_literal_new(g, sa));
    printf("/a?/\t\"%s\"\tmatch? %d\n", s0, _solPattern_is_match(pE_a, s0, l0));
    printf("/a?/\t\"%s\"\tmatch? %d\n", sa, _solPattern_is_match(pE_a, sa, l1));
    printf("/a?/\t\"%s\"\tmatch? %d\n", sb, _solPattern_is_match(pE_a, sb, l1));
    printf("/a?/\t\"%s\"\tmatch? %d\n", saaa, _solPattern_is_match(pE_a, saaa, l3));
    solPattern_free(pE_a);
    SolPattern *pE_abc = solPattern_choose(
        solPattern_empty_new(g),
        solPattern_concatenate(solPattern_concatenate(solPattern_literal_new(g, sa),
                                                      solPattern_literal_new(g, sb)),
                               solPattern_literal_new(g, sc)));
    printf("/(abc)?/\t\"%s\"\tmatch? %d\n", s0, _solPattern_is_match(pE_abc, s0, l0));
    printf("/(abc)?/\t\"%s\"\tmatch? %d\n", sa, _solPattern_is_match(pE_abc, sa, l1));
    printf("/(abc)?/\t\"%s\"\tmatch? %d\n", saaa, _solPattern_is_match(pE_abc, saaa, l3));
    printf("/(abc)?/\t\"%s\"\tmatch? %d\n", sabc, _solPattern_is_match(pE_abc, sabc, l3));
    printf("/(abc)?/\t\"%s\"\tmatch? %d\n", sabcabcabc, _solPattern_is_match(pE_abc, sabcabcabc, l9));
    solPattern_free(pE_abc);

    printf("\n------------ test capture-----------------\n");
    SolPattern *pM_abc = solPattern_capture(
        solPattern_concatenate(solPattern_literal_new(g, sa),
                               solPattern_literal_new(g, sb)),
        SolPatternCaptureMarkFlag_None,
        "M1");
    _solPattern_debug_dfa_relations(pM_abc);
    solPattern_set_reading_literal_func(pM_abc, &read_char);
    printf("/(M1:ab)/\t\"%s\"\tmatch? %d\n", sabc, solPattern_match(pM_abc, sabc, l3));
    print_match_result(pM_abc, sabc);
    solPattern_concatenate(pM_abc, solPattern_literal_new(g, sc));
    //_solPattern_debug_dfa_relations(pM_abc);
    printf("/(M1:ab)/\t\"%s\"\tmatch? %d\n", sabc, solPattern_match(pM_abc, sabc, l3));
    print_match_result(pM_abc, sabc);
    solPattern_free(pM_abc);
    SolPattern *pM_R_abc = solPattern_concatenate(
        solPattern_capture(solPattern_concatenate(solPattern_literal_new(g, sa),
                                                  solPattern_literal_new(g, sb)),
                           SolPatternCaptureMarkFlag_None,
                           "M1"),
        solPattern_literal_new(g, sc));
    solPattern_repeat(pM_R_abc);
    solPattern_set_reading_literal_func(pM_R_abc, &read_char);
    _solPattern_debug_dfa_relations(pM_R_abc);
    printf("/((M1:ab)c)*/\t\"%s\"\tmatch? %d\n", sabc, solPattern_match(pM_R_abc, sabc, l3));
    print_match_result(pM_R_abc, sabc);
    solPattern_free(pM_R_abc);
    solPatternStateGen_free(g);
    return 0;
}

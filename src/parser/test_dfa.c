#include <string.h>
#include <stdio.h>
#include "sol_utils.h"
#include "sol_dfa.h"

int _state_equal(void *s1, void *s2)
{
    if ((int*)s1 == (int*)s2) {
        return 0;
    }
    return 1;
}

int _character_equal(void *s1, void *s2)
{
    if (*(char*)s1 == *(char*)s2) {
        return 0;
    }
    return 1;
}

void _solDfa_debug_relations(SolDfa *d)
{
    printf("Starting state: (%d):\n", *(int*)solDfa_starting_state(d));
    printf("Accepting states:\n");
    void *acs;
    while ((acs = solSet_get(solDfa_accepting_states(d)))) {
        printf("state: %d\n", *(int*)acs);
    }
    printf("All states:\n");
    int *n;
    SolHashIter *i = solHashIter_new(solDfa_all_states(d));
    SolHashIter *j;
    SolDfaState *s;
    SolHashRecord *r;
    solHashIter_rewind(i);
    while ((r = solHashIter_get(i))) {
        n = (int*)r->k;
        s = r->v;
        if (solDfaState_rules(s)) {
            j = solHashIter_new(solDfaState_rules(s));
            solHashIter_rewind(j);
            while ((r = solHashIter_get(j))) {
                printf("rules: (%d) -(%c)-> (%d)\n", *n, *((char*)r->k), *(int*)(((SolDfaState*)r->v)->s));
            }
            solHashIter_free(j);
        }
    }
    solHashIter_free(i);
}

void print_current_state(SolDfa *d)
{
    if (solDfa_current_state(d)) {
        printf("current state is %d\n", *(int*)(solDfa_current_state(d)));
    } else {
        printf("current state empty\n");
    }
    if (solDfa_is_accepting(d) == 0) {
        printf("ACCEPTING\n");
    }
}

int main()
{
    int i = 1;
    int ii = 2;
    int iii = 3;
    int iv = 4;
    char a = 'a';
    char b = 'b';
    char c = 'c';
    SolDfa *d = solDfa_new(&sol_i_hash_func1, &sol_i_hash_func2, &_state_equal,
                           &sol_c_hash_func1, &sol_c_hash_func2, &_character_equal);
    solDfa_set_starting_state(d, &i);
    solDfa_add_accepting_state(d, &iv);
    if (solDfa_add_rule(d, &i, &ii, &a) != 0) {
        goto err;
    }
    if (solDfa_add_rule(d, &i, &iii, &b) != 0) {
        goto err;
    }
    if (solDfa_add_rule(d, &ii, &iv, &c) != 0) {
        goto err;
    }
    if (solDfa_add_rule(d, &iii, &iv, &c) != 0) {
        goto err;
    }
    _solDfa_debug_relations(d);
    printf("read %c\n", a);
    solDfa_read_character(d, &a);
    print_current_state(d);

    printf("read %c\n", a);
    solDfa_read_character(d, &a);
    print_current_state(d);

    printf("read %c\n", b);
    solDfa_read_character(d, &b);
    print_current_state(d);

    printf("reset current state\n");
    solDfa_reset_current_state(d);
    print_current_state(d);

    printf("read %c\n", a);
    solDfa_read_character(d, &a);
    print_current_state(d);

    printf("read %c\n", c);
    solDfa_read_character(d, &c);
    print_current_state(d);
    solDfa_free(d);
    return 0;
 err:
    solDfa_free(d);
    return 1;
}

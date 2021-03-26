#include "../include/nfa.h"
#include <stdint.h>
#include <stdio.h>
#include "../include/debug.h"

extern struct nfa *nfa_array;

void print_states(struct set *s) {
    if(!s) {
        printf("NULL");
        return;
    }
    setnextmember(NULL);
    int i = setnextmember(s);
    putchar('{');
    while(i != -1) {
        putchar('0'+i);
        i = setnextmember(s);
        if(i != -1) putchar(',');
    }
    putchar('}');
    fflush(stdout);
}

struct set *e_closure(struct set *s, char **accept, int *anchor) {
    if(!s) return s;
    *accept = NULL;
    *anchor = 0;
    D(printf("e-closure(");)
    D(print_states(s);)
    D(printf(")");)
    int accept_line = INT32_MAX;
    int stack[MAXNFA];
    int *stack_p = &stack[MAXNFA];
    setnextmember(NULL);
    int i;
    while((i = setnextmember(s)) != -1) {
        *--stack_p = i;

    }
    while(stack_p < &stack[MAXNFA]) {
        struct nfa *cur = &nfa_array[*stack_p++];
        if(cur->accept) {
            int cur_line = ((int *)(cur->accept))[-1];
            if(cur_line < accept_line) {
                *accept = cur->accept;
                *anchor = cur->anchor;
                accept_line = cur_line;
            }
        }
        if(cur->edge == EPSILON) {
            if(cur->next) {
                i = (int) (cur->next - nfa_array);
                if(!setcontains(s, i)) {
                    setadd(s, i);
                    *--stack_p = i;
                }
            }
            if(cur->next2) {
                i = (int) (cur->next2 - nfa_array);
                if(!setcontains(s, i)) {
                    setadd(s, i);
                    *--stack_p = i;
                }
            }
        }
    }
    D(printf(" = ");)
    D(print_states(s);)
    D(printf("\n");)
    return s;
}


struct set *nfa_move(struct set *s, int c) {
    D(printf("move(");)
    D(print_states(s);)
    D(printf(", `%c`)", c);)
    struct set *out = NULL;
    setnextmember(NULL);
    int i;
    while((i = setnextmember(s)) != -1) {
        struct nfa *cur = &nfa_array[i];
        if(cur->edge == c || (cur->edge == CCL && setcontains(cur->ccl, c))) {
            if(!out) {
                out = setcreate();
            }
            setadd(out, (cur->next-nfa_array));
        }
    }
    D(printf(" = ");)
    D(print_states(out);)
    D(printf("\n");)
    return out;
}
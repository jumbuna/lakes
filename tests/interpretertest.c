#include "../include/nfa.h"
#include <assert.h>
#define MAIN
#include "../include/globals.h"
#undef MAIN

extern struct set *e_closure(), *nfa_move();

extern char *test_expression();
extern struct nfa *nfa_array;

int main() {
    struct nfa *start;
    int size;
    int ret = thompson_construction(&nfa_array, &start, &size, test_expression);
    if(ret != N_OK) {
        print_nerror();
        return ret;
    }
    struct set *ss = setcreate();
    setadd(ss, (start-nfa_array));
    char *accept;
    int anchor;
    ss = e_closure(ss, &accept, &anchor);
    assert(setsize(ss) == 8);
    struct set *t = nfa_move(ss, '8');
    assert(t);
    assert(setsize(t) == 1);
    assert(setsize(e_closure(t, &accept, &anchor)) == 3);
    assert(!nfa_move(t, '9'));
}
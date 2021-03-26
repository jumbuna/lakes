#include "../include/nfa.h"
#include <assert.h>
#define MAIN
#include "../include/globals.h"
#undef MAIN

extern struct set *e_closure(), *nfa_move();

char *date = "(0?[0-9]|[1-2][0-9]|3[0-1])/([0-9]|1[1-2])/([0-9][0-9][0-9][0-9]|[0-9][0-9]) return date;";

struct nfa *nfa_array;

char *test_expression() {
    static int firstrun = 1;
    if(firstrun) {
        firstrun = 0;
        return date;
    }
    return 0;
}

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
#include <stdio.h>
#include "../include/debug.h"
#include "/usr/local/include/set/set.h"
#include "../include/globals.h"

// EDGES
#define EPSILON     -1
#define CCL         -2
#define EMPTY       -3

// ANCHORAGE
#define NONE        0
#define START       1
#define END         2
#define BOTH        3

typedef struct set cc;

struct nfa  {
    int edge, anchor;
    struct nfa *next, *next2;
    cc *ccl;
    char *accept;
};

int thompson_construction(struct nfa **nfa, struct nfa **start, char *(*inputfun)());




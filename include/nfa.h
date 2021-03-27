#include <stdio.h>
#include "../include/debug.h"
#include "/usr/local/include/set/set.h"
#include "../include/globals.h"

#define MAXNFA 1024
#define MAXACCEPT 2048 * 10

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

int thompson_construction(struct nfa **, struct nfa **, int *, char *(*)());

enum nerror {
    N_OK,   // ok
    N_TOOMANYNFAS, // Too many nfas
    N_MISSNGPAREN, // missing ')'
    N_NESTTOODEEP, // Too many nested macros
    N_NFASNOMEM,   // nfas == NULL
    N_ACCEPTSNOMEM,   //accepts == NULL
    N_COMPLEXEXPR, // complex expression
    N_BADCLOSURE, // unbinded closure
    N_BADBOL, // invalid `^`
    N_NOCCL,    // unmatched `]`
    N_OPENCCL, // missing `]` 
    N_NOMACRO,
};

extern char *nerror2str[];

void print_nerror();

int get_nerror();
#include "nfa.h"

typedef int ROW[128];

#define MAXDFA 512

int make_dfa(), minimize_dfa(), compress_dfa();

enum derror {
    D_OK,
    D_DFANOMEM,
    D_TOOMANYDFAS,
    D_NOMEMAA,
    D_NOMEMDTRAN
};

//Accept-Anchor
struct AA {
    char *accept;
    int anchor;
};

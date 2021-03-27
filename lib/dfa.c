#include "../include/dfa.h"
#include <stdio.h>
#include <stdlib.h>

#define F -1
#define MAXCHAR 127

extern struct nfa *nfa_array;
extern void print_states();
extern struct set *e_closure(), *nfa_move();

struct dfa {
    struct set *ds;
    char *acc;
    int anc;
    int marked;
};

PRIVATE struct dfa *dfas;
ROW *dtran;
struct AA *aa;
PRIVATE int next_alloc;
PRIVATE int last_error;

PRIVATE int new_dfa(struct set *s, char *acc, int anc, int *p) {
    RETURN_ON_ERROR_WITH_VALUE(-1);
    static int firstrun = 1;
    if(firstrun) {
        firstrun = 0;
        if(!(dfas = calloc(MAXDFA, sizeof(struct dfa)))) {
            ERROR(D_DFANOMEM);
            RETURN_ON_ERROR_WITH_VALUE(-1);
        }
        next_alloc = 0;
    }
    if(next_alloc >= MAXDFA) {
        ERROR(D_TOOMANYDFAS);
        RETURN_ON_ERROR_WITH_VALUE(-1);
    }
    dfas[next_alloc].ds = s;
    dfas[next_alloc].acc = acc;
    dfas[next_alloc].anc = anc;
    *p = next_alloc++;
    return last_error;
}

PRIVATE int in_dfas(struct set *s) {
    RETURN_ON_ERROR_WITH_VALUE(-1);
    for(int i = 0; i < next_alloc; i++) {
        if(setsequivalent(dfas[i].ds, s)) return i;
    }
    return -1;
}

PRIVATE int make_dtran() {
    RETURN_ON_ERROR_WITH_VALUE(-1);
    char *accept;
    int anchor;
    int nextstate;
    struct set *t;
    for(int i = 0; i < next_alloc; i++) {
        for(int j = 0; j <= MAXCHAR; j++) {
            t = e_closure(nfa_move(dfas[i].ds, j), &accept, &anchor);
            if(!t) {
                nextstate = F;
            } else {
                if((nextstate = in_dfas(t)) == -1) {
                    new_dfa(t, accept, anchor, &nextstate);
                    RETURN_ON_ERROR_WITH_VALUE(-1);
                } else {
                    setdestroy(t);
                }
                
            }
            dtran[i][j] = nextstate;
        }
    }
    return last_error;
}

int make_dfa(ROW **table, struct AA **acc, int *size, char *(*ifun)()) {
    struct nfa *start;
    if(thompson_construction(&nfa_array, &start, size, ifun) != N_OK) {
        RETURN_ON_ERROR_WITH_VALUE(-1);
    }
    int t;
    new_dfa(0, 0, 0, &t);
    RETURN_ON_ERROR_WITH_VALUE(-1);
    struct set *s = setcreate();
    setadd(s, (start-nfa_array));
    dfas[0].ds = e_closure(s, &dfas[0].acc, &dfas[0].anc);
    if(!(dtran = malloc(MAXDFA*sizeof(ROW)))) {
        ERROR(D_NOMEMDTRAN);
        RETURN_ON_ERROR_WITH_VALUE(last_error);
    }
    make_dtran();
    RETURN_ON_ERROR_WITH_VALUE(last_error);
    if(!(dtran = realloc(dtran, next_alloc*sizeof(ROW)))) {
        ERROR(D_NOMEMDTRAN);
        RETURN_ON_ERROR_WITH_VALUE(last_error);
    }
    if(!(aa = malloc(next_alloc * sizeof(struct AA)))) {
        ERROR(D_NOMEMAA);
        RETURN_ON_ERROR_WITH_VALUE(last_error);
    }
    for(int i = 0; i < next_alloc; i++) {
        aa[i].accept = dfas[i].acc;
        aa[i].anchor = dfas[i].anc;
    }
    free(nfa_array);
    free(dfas);
    *table = dtran;
    *size = next_alloc;
    *acc = aa;
    return last_error;
}
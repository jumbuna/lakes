#include "../include/debug.h"
#include "../include/globals.h"
#include <ctype.h>

#define MAXINPUT 2048

PRIVATE int get_line(char **p, int *n) {
    static int lookahead = 0;
    if(!lookahead) {
        lookahead = getc(Ifile);
    }
    char *t = *p;
    if(--*n > 1 && lookahead != EOF) {
        while(--*n >= 0) {
            *t = lookahead;
            lookahead = getc(Ifile);
            if(*t == '\n' || *t == EOF) {
                *t = 0;
                break;
            }
            t++;
        }
        *p = t;
    }
    return *n ? lookahead : 0;
}

char *get_expr() {
    static int lookahead;
    static char buffer[MAXINPUT];
    if(lookahead == EOF) {
        return NULL;
    }
    if(lookahead == '%') {
        return NULL;
    }
    Lineno = Actuallineno;
    char *s = buffer;
    int sl = MAXINPUT;
    while((lookahead = get_line(&s, &sl)) != EOF) {
        if(lookahead == 0) {
            // TODO Error
        }
        ++Actuallineno;
        if(!buffer[0]) continue; //blank line
        if(!isspace(lookahead)) break;
        *s++ = '\n';
    }
    return buffer;
}
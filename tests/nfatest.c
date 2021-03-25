#include "../include/nfa.h"
#include <stdio.h>
#define MAIN
#include "../include/globals.h"
#undef MAIN

extern int pre_expr();
extern char *get_expr();

int main() {
    Ifile = fopen("/Users/jumbuna/Developer/lakes/tests/demo.lex", "r");
    Ofile = stdout;
    pre_expr();
    struct nfa *start, *arr;
    int size = thompson_construction(&arr, &start, get_expr);

}
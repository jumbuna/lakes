#include "../include/nfa.h"
#include <stdio.h>
#define MAIN
#include "../include/globals.h"
#undef MAIN

extern int pre_expr();
extern char *get_expr();

int main(int argc, char **argv) {
    Ifile = fopen(Ifilename = argv[1], "r");
    Ofile = stdout;
    pre_expr();
    struct nfa *start, *arr;
    int size;
    int ret = thompson_construction(&arr, &start, &size, get_expr);
    print_nerror();
    return ret;
}
#include "../include/dfa.h"
#include <stdio.h>
#define MAIN
#include "../include/globals.h"
#undef MAIN
#include <stdlib.h>
#include <assert.h>

extern int print_header();
extern char *test_expression();

int main() {
    ROW *r;
    struct AA *aa;
    int size;
    Ifilename = "/mem";
    int ret = make_dfa(&r, &aa, &size, test_expression);
    if(ret < 0) {
        print_nerror();
        exit(get_nerror());
    }
    if(ret > 0) {
        printf("%d\n", ret);
        return ret;
    }
    Ofile = stdout;
    int *accepts = malloc(size * sizeof(int));
    for(int i = 0; i < size; i++) {
        accepts[i] = aa[i].accept != NULL;
    }
    print_header(r, accepts, size);
}
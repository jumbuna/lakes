#include <stdio.h>
#include <stdlib.h>

enum token {
    LITERAL,
    CLOSURE,
    OPTIONAL,
    PLUS,
    ANY,
    LBRACE,
    RBRACE,
    LPAREN,
    RPAREN,
    LCCL,
    RCCL,
    DASH,
    EOI,
    EOS,
    BOL,
    EOL,
    OR
};

char *tk2str[] = {
    "LITERAL",
    "CLOSURE",
    "OPTIONAL",
    "PLUS",
    "ANY",
    "LBRACE",
    "RBRACE",
    "LPAREN",
    "RPAREN",
    "LCCL",
    "RCCL",
    "DASH",
    "EOI",
    "EOS",
    "BOL",
    "EOL",
    "OR"
};

int main(int argc, char **argv) {
    int columns_per_row = argc == 2 ? strtol(argv[1], NULL, 10) : 16;
    
    for(int i = 0; i < 128; i++) {
        if(!(i%columns_per_row)) printf("\n");
        switch(i) {
            case '+': printf("%9s", tk2str[PLUS]); break;
            case '*': printf("%9s", tk2str[CLOSURE]); break;
            case '-': printf("%9s", tk2str[DASH]); break;
            case '?': printf("%9s", tk2str[OPTIONAL]); break;
            case '.': printf("%9s", tk2str[ANY]); break;
            case '[': printf("%9s", tk2str[LCCL]); break;
            case ']': printf("%9s", tk2str[RCCL]); break;
            case '(': printf("%9s", tk2str[LPAREN]); break;
            case ')': printf("%9s", tk2str[RPAREN]); break;
            case '|': printf("%9s", tk2str[OR]); break;
            case '$': printf("%9s", tk2str[EOL]); break;
            case '^': printf("%9s", tk2str[BOL]); break;
            default: printf("%9s", tk2str[LITERAL]);
        }
        if(i != 127) printf(", ");
    }
    printf("\n");
}
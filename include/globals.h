#ifdef MAIN
#define ALLOC
#define I(x) x
#else
#define ALLOC extern
#define I(x)
#endif

#include <stdio.h>

/**
 * input file
 */
ALLOC FILE *Ifile;
/**
 * output file
 */
ALLOC FILE *Ofile;
/**
 * template file
 */
ALLOC FILE *Template;
/**
 * Line number of Lexeme
 */
ALLOC int Lineno I(=0);
ALLOC int Actuallineno I(=0);
ALLOC int Lexeme;
ALLOC char *Ifilename;
ALLOC char *Ofilename I(="lex.yy.c");
ALLOC char *Tname I(="lex.parts.c");


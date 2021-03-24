// © jumbuna, 2021
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YYPRIVATE static
#define YYPUBLIC

/**
 * output errors to stderr
 */
#define yy_error(...)	(fprintf(stderr, __VA_ARGS__))

/**
 * lookahead character
 */
YYPRIVATE int yy_look;

/**
 * current state the dfa is in
 */
YYPRIVATE int yy_state;

/**
 * next state the dfa will be in
 */
YYPRIVATE int yy_nextstate;

/**
 * last accept state
 */
YYPRIVATE int yy_lastaccept;

/**
 * previous state
 */
YYPRIVATE int yy_prev;

// parser communication variables

/**
 * lexeme of the most recent token
 */
YYPUBLIC char *yy_lexeme;

/**
 * line number of yy_lexeme
 */
YYPUBLIC int yy_lineno;

/**
 * set the yyflag
 */
#define yy_more() yyflag = 1

/**
 * force the lexer to ignore the last accept state
 * and continue processing.
 */
YYPRIVATE int yyflag;






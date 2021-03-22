/*
 *	© jumbuna, 2021
 */
#ifndef _MACRO_H
#define _MACRO_H
#include <map/map.h>
#include <sys/errno.h>
#include <stdio.h>

/* Error codes */
#define M_OK						0
#define M_NAME_2_LONG 	-1
#define M_TEXT_2_LONG 	-2
#define M_CURLY_MISS		-3
#define M_MACRO_MISS		-4


/* Limits */
#define M_NAME_LIMIT  32
#define M_TEXT_LIMIT  64

/**
 * Save a macro definition.
 * Constraints:
 * - Macro name should not exceed 31 characters.
 * - Macro text should not exceed 63 characters.
 * Returns 0 on success or any other number if
 * an error occurred.
 */
int macro_save(char *);

/**
 * Retrieve a macro expansion if possible
 * and advance the indirect pointer to
 * point to the end of the macro name.
 * Return pointer to macro text or NULL if
 * macro doesn't exist.
 */
int macro_get(char **, char **);

/**
 * Number of macros currently saved
 */
int macro_count();

/**
 * Output all macros to the given FILE stream
 */
void macro_output(FILE *);
#endif // end _MACRO_H

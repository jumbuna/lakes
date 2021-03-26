#include <stdio.h>
#include "../include/debug.h"
#include "../include/dfa.h"
#include <ctype.h>
#include "../include/macro.h"
#include "../include/globals.h"
#include "../project_config.h"

#define TABSTOP	4


void signon() {
	FILE *d;
	/* Unix only */
	if(!(d = fopen("/dev/tty", "w")))
		d = stderr;
	fprintf(d, "%s v%d.%d Jumbuna. All rights reserved, ©%s\n\n", PROJECTNAME, VER_MAJOR, VER_MINOR, &(__DATE__[7]));
	if(d != stderr) {
		fclose(d);
	}
}

static char buffer[256];
/**
 * process contents of the lex file upto the beginning of
 * the regular expressions
 */
int pre_expr() {
	int transparent = 0;
	while(fgets( buffer, 256, Ifile)) {
		++Lineno;
		if(buffer[0] == '%') {
			if(buffer[1] == '{') {
				transparent = 1;
			} else if(buffer[1] == '}') {
				transparent = 0;
			} else if(buffer[1] == '%') {
				break;
			} else {
				fprintf(stderr, "ignoring invalid %%%c directive on line #%d", buffer[1], Lineno);
			}
			continue;
		}
		if(transparent) {
			fprintf(Ofile, "%s", buffer);
			continue;
		}
		char *temp = buffer;
		while(isspace(*temp)) temp++;
		/* comment */
		if(*temp == '@') {
			fputc('\n', Ofile);
		} else if(*temp) {
			macro_save(temp);
			fputc('\n', Ofile);
		}
	}
	Actuallineno = Lineno;
	return !feof(Ifile);
}

int print_yynext(char *name) {
	fprintf(Ofile, "/** next state */\n");
	return fprintf(Ofile, "#define yy_next(s, c) yy_%s[s][c];\n", name);
}

/**
 * print a header comment describing the
 * dfa
 */
int print_header(ROW *dfa, int *accepts, int n) {
	fprintf(Ofile, "#ifdef __NEVER\n");
	fprintf(Ofile, "/**------------------------------------\n");
	fprintf(Ofile, " * Dfa start state is 0:\n *\n");
	for(int i = 0; i < n; i++) {
		fprintf(Ofile, " * State %d [%saccepting]:", i, accepts[i] ? "" : "non");
		int laststate = -1;
		int p = 0;
		for(int j = 1; j < 128; j++) {
			if(dfa[i][j] != -1) {
				if(dfa[i][j] == laststate) {
					fprintf(Ofile, "%c", j);
					if(++p > 56) {
						fprintf(Ofile, "\n * %*s", 10, "");
						p = 0;
					}	
				} else {
					fprintf(Ofile, "\n * goto %d on %c", dfa[i][j], j);
					laststate = dfa[i][j];
					p = 0;
				}
			}
		}
		fprintf(Ofile, "\n");
	}
	fprintf(Ofile, " *\n */\n");
	return fprintf(Ofile, "#endif //__NEVER\n");
}

int print_templatepart() {
	while(fgets(buffer, 256, Template)) {
		if(buffer[0] == '\f') {
			break;
		} else {
			fprintf(Ofile, "%s", buffer);
		}
	}
	return !feof(Template);
}

int print_dfa(ROW *dfa, int n, int padding) {
	fprintf(Ofile, "%*sint yy_dfa[%d][%d] = {\n", padding, "", n, 128);
	for(int i = 0; i < n; i++) {
		fprintf(Ofile, "%*s/* state %d */\n", TABSTOP+padding, "", i);
		fprintf(Ofile, "%*s{", TABSTOP+padding, "");
		for(int j = 0; j < 128; j++) {
			if(!(j%16)) {
				fprintf(Ofile, "\n\%*s", TABSTOP*2+padding, "");
			}
			fprintf(Ofile, "%2d, ", dfa[i][j]);
		}
		fprintf(Ofile, "\n%*s},\n", TABSTOP+padding, "");
	}
	fprintf(Ofile, "%*s};\n", padding, "");
	return 1;
}

int print_accepts(int *accepts, int n, int padding) {
	fprintf(Ofile, "%*s/* Comment goes here */\n", padding, "");
	fprintf(Ofile, "%*sunsigned int yy_accept[%d] = {\n", padding, "", n);
	for(int i = 0; i < n; i++) {
		fprintf(Ofile, "%*s%d, /* STATE %d */\n", TABSTOP+padding, "", i, i);
	}
	return fprintf(Ofile, "%*s}\n;", padding, "");
}



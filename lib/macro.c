
/*
 *	© jumbuna, 2021
 */

#include <string.h>
#include <ctype.h>
#include <macro.h>
#include <debug.h>

PRIVATE struct Map *macromap;

extern int hashadd();

struct macro {
	char name[M_NAME_LIMIT];
	char text[M_TEXT_LIMIT];
};

int macro_save(char macrodef[]) {
	if(!macromap)	macromap = mapcreate(32, hashadd, strcmp, sizeof(struct macro));
	char *text, *name;
	name = macrodef;
	while(!isspace(*macrodef)) macrodef++;
	*macrodef++ = '\0';
	if((macrodef-name) >= M_NAME_LIMIT) return M_NAME_2_LONG;
	while(isspace(*macrodef)) macrodef++;
	text = macrodef;
	char *edef = NULL;
	while(*macrodef) {
		if(isspace(*macrodef))
			for(edef = macrodef; isspace(*macrodef) && *macrodef; ++macrodef);
	 	else	macrodef++;
	}
	if(edef) {
		*edef = '\0';
		if((edef-text) >= M_TEXT_LIMIT) return M_TEXT_2_LONG;
	}
	D(printf("macro name: %s\n", name);)
	D(printf("macro text: %s\n", text);)
	struct macro *entry, *temp;
	if(!(entry = temp =  mapfindentry(macromap, name))) entry =  mapentrycreate(macromap);
	if(!entry) return errno;
	if(!temp) strcpy(entry->name, name);
	strcpy(entry->text, text);
	mapaddentry(macromap, entry);
	return 0;
}

int macro_get(char **macroname, char **macrotext) {
	char *temp;
	if(!(temp = strchr(++*macroname, '}'))) return M_CURLY_MISS;
	*temp++ = '\0';
	struct macro *t = mapfindentry(macromap, *macroname);
	if(!t) return M_MACRO_MISS;
	*macrotext = t->text;
	*macroname = temp;
	return M_OK;
}

int macro_count() {
	return macromap ? mapsize(macromap) : 0;
}

PRIVATE int _macro_print(FILE *stream, struct macro *m) {
	return fprintf(stream, "%s : %s\n", m->name, m->text);
}

void macro_output(FILE *stream) {
	mapprint(macromap, _macro_print, stream);
}

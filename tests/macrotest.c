#include <macro.h>
#include <stdlib.h>
#include <debug.h>
#include <assert.h>

char *macro;// = "Hello world \n";
char *name;// = "{Hello}world";
char *text;

int main() {
	macro = malloc(50);
	strcpy(macro, "Hello World ! \n");
	name = macro+20;
	strcpy(name, "{Hello}world"); 
	assert(macro_save(macro) == M_OK);
	assert(macro_count() == 1);
	assert(macro_get(&name, &text) == M_OK);
	assert(!strcmp("world", name));
	macro_output(stdout);	
}

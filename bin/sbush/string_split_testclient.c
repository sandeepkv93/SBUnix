#include<stdio.h>
#include<string.h>
#include "stringll.h"
#include "stringlib.h"

#ifdef __TEST__
int main() {
	char *inp;
	struct stringllnode* out = NULL;
	strcpy(inp,"this is &   ");
	lib_str_remove_extra_spaces(inp);
	printf("%s<-\n",inp);
	lib_str_split(inp,"&", &out);
	print_list(out);
	printf("%d", lib_str_find(inp, '|'));
}
#endif

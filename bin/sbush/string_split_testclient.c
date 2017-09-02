#include<stdio.h>
#include "stringll.h"
#include "stringlib.h"

int main() {
	char *inp = "ls -l | grep hello | more";
	struct stringllnode* out = lib_str_split(inp,"|");
	print_list(out);
}
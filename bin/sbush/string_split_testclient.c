#include "stringlib.h"
#include "stringll.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __TEST__
int
main()
{
    char* inp;
    struct stringllnode* out = NULL;
    strcpy(inp, "thi  | is kjslks");
    lib_str_remove_extra_spaces(inp);
    printf("%s<-\n", inp);
    lib_str_split(inp, '|', &out);
    print_list(out);
    printf("Found at %d\n", lib_str_find(inp, "is"));
}
#endif

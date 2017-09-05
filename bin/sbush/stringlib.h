#include "stringll.h"

int lib_str_split(const char *,char *, struct  stringllnode **);
int lib_str_split_get_member(const char *string_, char *delimiter, int index, char* a);
void lib_str_remove_extra_spaces(char *);
int lib_str_find(char * str, char * x);
int strcmp_(const char * s1,const char * s2);
int strlen_(const char * s1);
char * strcpy_(char * dest,const char * src);
char * strcat_(char * dest,const char * src);

#ifndef _STRING_H
#define _STRING_H
int strcmp(const char* s1, const char* s2);
int strlen(const char* s1);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, int n);
char* strcat(char* dest, const char* src);
void charcpy(char * dest, char * src, int char_num);
void longcpy(long * dest, long * src, int long_num);
void memcpy(void * dest, void * src, int size);
#endif

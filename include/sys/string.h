#ifndef _STRING_H
#define _STRING_H
int strcmp(const char* s1, const char* s2);
int strlen(const char* s1);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, int n);
char* strcat(char* dest, const char* src);
void memcpy(void* dest, void* src, int size);
void* memset(void* s, int c, int n);
char* strchr(char* s, char p);
char* strrev(char* str);
char* strrchr(char* s, char p);
#endif

#include <sys/defs.h>
#include <sys/string.h>

void*
memset(void* s, int c, int n)
{
    unsigned char* p = s;
    while (n) {
        *p++ = (unsigned char)c;
        n--;
    }
    return s;
}

void
charcpy(char* dest, char* src, int char_num)
{
    while (char_num--) {
        *dest++ = *src++;
    }
}

void
longcpy(long* dest, long* src, int long_num)
{
    while (long_num--) {
        *dest++ = *src++;
    }
}

void
memcpy(void* dest, void* src, int size)
{
    int char_num = size % sizeof(long);
    int long_num = size / sizeof(long);
    int char_start = 0;
    if (long_num) {
        longcpy((long*)dest, (long*)src, long_num);
        char_start = size - char_num;
    }
    char_start = size - char_num;
    if (char_num) {
        charcpy((char*)(dest + char_start), (char*)(src + char_start),
                char_num);
    }
}

int
strcmp(const char* s1, const char* s2)
{
    while (*s1 == *s2 && *s1 != '\0') {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

int
strncmp(const char* s1, const char* s2, int n)
{
    while (n > 0 && *s1 == *s2 && *s1 != '\0') {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) {
        return 0;
    }
    return *s1 - *s2;
}

int
strlen(const char* s1)
{
    int len = 0;
    while (*s1++ != '\0') {
        len++;
    }
    return len;
}

char*
strcpy(char* dest, const char* src)
{
    char* temp;
    temp = dest;
    while ((*temp++ = *src++) != '\0')
        ;
    return dest;
}

char*
strncpy(char* dest, const char* src, int n)
{
    char* temp;
    temp = dest;
    while (n && (*temp++ = *src++) != '\0') {
        n--;
    }
    return dest;
}

char*
strcat(char* dest, const char* src)
{
    char* temp = dest;
    while (*temp != '\0') {
        temp++;
    }
    while ((*temp++ = *src++) != '\0')
        ;
    *temp = '\0';
    return dest;
}

char*
strchr(char* s, char p)
{
    while (*s) {
        if (*s == p) {
            return (char*)s;
        }
        s++;
    }
    return NULL;
}

char*
strrev(char* str)
{
    char *p1, *p2;

    if (!str || !*str)
        return str;
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}

char*
strrchr(char* s, char p)
{
    char* ret = NULL;
    while (*s) {
        if (*s == p) {
            ret = (char*)s;
        }
        s++;
    }
    return ret;
}
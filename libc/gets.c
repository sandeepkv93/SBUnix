#include <stdio.h>
#include <unistd.h>

char*
fgets(int fd, char* s)
{
    int end = read(fd, s, 999);
    s[end] = 0;
    return s;
}

char*
gets(char* s)
{
    return fgets(0, s);
}

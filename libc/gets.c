#include <stdio.h>
#include <unistd.h>

char*
fgets(int fd, char* s)
{
    read(fd, s, 999);
    return s;
}

char*
gets(char* s)
{
    return fgets(0, s);
}

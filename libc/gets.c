#include <stdio.h>
#include <unistd.h>

char*
fgets(int fd, char* s)
{
    int i = 0;
    char a = '\0';
    while (1) {
        if (read(fd, &a, 1) <= 0) {
            return NULL;
        }

        if (a == '\n' || a == EOF) {
            break;
        }

        s[i] = a;
        i++;
    }
    s[i] = '\0';
    return s;
}

char*
gets(char* s)
{
    return fgets(0, s);
}

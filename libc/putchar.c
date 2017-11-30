#include <stdio.h>
#include <unistd.h>

int
putchar(int c)
{
    // write character to stdout
    char* a;
    yield();
    a = (char*)&c;
    write(1, a, 1);
    return c;
}

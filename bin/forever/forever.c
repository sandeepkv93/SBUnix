
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

int
main(int argc, char** argv)
{
    printf("Starting while(1); loop\n");
    while (1)
        ;
}

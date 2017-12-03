#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define F_OK 0

int
main(int argc, char** argv, char** envp)
{
    int i;
    char buff[100];
    // char* p = (char*)malloc(sizeof(char) * 5);
    // uint64_t addr = 18446744071537491968U;
    puts("hi");
    // p[0] = 'H';
    // p[1] = 'I';
    // p[2] = '\0';
    exit(0);
    // puts(p);

    // brk((void*)NULL);
    puts("Hey!\n");
    while (1)
        yield();

    for (i = 0; i < argc; i++) {
        puts(argv[i]);
    }

    for (i = 0; envp[i] != NULL; i++) {
        puts(envp[i]);
    }

    gets(buff);
    puts(buff);
    //	puts(buff);
    if (access(buff, F_OK) < 0) {
        puts("Not found");
    } else {
        puts("Found");
    }
    return 0;
}

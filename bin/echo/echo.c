#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

int
main(int argc, char** argv)
{
    int i = 1;
    if (argc > 1) {
        while (i < argc) {
            write(1, argv[i], strlen(argv[i]));
            write(1, " ", 1);
            ++i;
        }
    } else {
        puts("Usage : echo <string> [<string> ..]");
    }
    puts(" ");
}

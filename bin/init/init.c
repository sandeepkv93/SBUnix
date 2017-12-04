#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int
main2(int argc, char** argv, char** envp)
{
    int x = 1;
    x = fork();
    if (x) {
        putchar('P');
    } else {
        putchar('c');
        x = fork();
        if (x) {
            putchar('C');
        } else {
            putchar('G');
            putchar('G');
            putchar('G');
            putchar('G');
        }
    }
    while (1)
        yield();
}

int
main1(int argc, char** argv, char** envp)
{
    char* argv_new[] = { "/bin/sbush", "/etc/rc", NULL };
    char* envp_new[] = { "PATH=/", NULL };

    int ret;
    puts("[ INIT started ]");
    ret = fork();
    if (!ret) {
        execvpe("/bin/sbush", argv_new, envp_new);
    }
    while (1) {
        // wait()
        yield();
    }
}

int
main(int argc, char** argv, char** envp)
{
    char* argv_new[] = { "/bin/sbush", "/etc/rc", NULL };
    char* envp_new[] = { "PATH=/", NULL };
    puts("[ INIT started ]");
    execvpe("/bin/init", argv_new, envp_new);
    return 0;
}

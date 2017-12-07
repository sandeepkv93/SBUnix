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
main3(int argc, char** argv, char** envp)
{
    char* argv_new[] = { "/bin/sbush", "/etc/rc", NULL };
    char* envp_new[] = { "PATH=/", "PWD=/", NULL };
    printf("Hello World ! %d \n", 2);
    execvpe("/bin/init", argv_new, envp_new);
    return 0;
}

int
main(int argc, char** argv, char** envp)
{
    char* argv_new[] = { NULL };
    char* envp_new[] = { "PATH=/bin/", "PWD=/", NULL };
    int status;
    pid_t child_pid, term_pid;
    puts("[ INIT started ]");
    while (1) {
        child_pid = fork();
        if (!child_pid) {
            execvpe("/etc/rc", argv_new, envp_new);
            puts("Please ensure /etc/rc has the right syntax.");
        }
        while (1) {
            term_pid = wait(&status);
            if (term_pid == child_pid) {
                break;
            }
        }
    }
    return 0;
}

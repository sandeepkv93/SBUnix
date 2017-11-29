#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int
main(int argc, char** argv, char** envp)
{
    char* argv_new[] = { "/bin/sbush", NULL };
    char* envp_new[] = { "PATH=/", NULL };

    int ret;
    puts("[ INIT started ]");
    ret = fork();
    if (!ret) {
        execvpe("/bin/sbush", argv_new, envp_new);
    }
    while (1)
        yield();
}

#if 0
int
main2(int argc, char** argv, char** envp)
{
    char buff[100];

    char buff2[8] = { '0', ' ', 'a', 'r', 'g', 'c', '\n', '\0' };
    buff2[0] += argc;
    write(1, buff2, 7);

    char* init_msg = "Init has been started\n";
    write(2, init_msg, strlen(init_msg));

    char* args = "Here are the args: \n";
    write(2, args, strlen(args));
    for (int i = 0; argv[i] != NULL; i++) {
        write(1, argv[i], strlen(argv[i]));
        write(1, " ", 2);
    }
    write(1, "\n", 1);

    char* envs = "Here are the envs: \n";
    write(2, envs, strlen(envs));
    for (int i = 0; envp[i] != NULL; i++) {
        write(1, envp[i], strlen(envp[i]));
        write(1, " ", 1);
    }
    write(1, "\n", 1);

    //==============
    char bluff[100] = { 0 };
    int fd = open("bin/test.txt", 0);
    read(fd, bluff, 100);
    write(1, bluff, strlen(bluff));
    //=============

    while (1) {
        for (int i = 0; i < 20; i++)
            buff[i] = 0;
        write(1, "> ", 2);
        read(0, buff, 100);
        write(1, buff, strlen(buff));
        write(1, "\n", 1);
    }
    return 0;
}
#endif

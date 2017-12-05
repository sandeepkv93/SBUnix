#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

uint64_t
charArrayToInt(char* array)
{
    uint64_t number = 0;
    int mult = 1;
    int n = strlen(array);
    while (n--) {
        if ((array[n] < '0' || array[n] > '9') && array[n] != '-') {
            return 0;
        } else {
            number += (array[n] - '0') * mult;
            mult *= 10;
        }
    }
    return number;
}

void
print_help()
{
    puts("Usage : kill -9 <pid>");
    exit(1);
}
int
main(int argc, char** argv)
{
    pid_t pid;

    if ((argc == 3)) {

        if (strcmp(argv[1], "-9")) {
            print_help();
        }

        pid = charArrayToInt(argv[2]);

        if (pid == 0) {
            print_help();
        }

        return kill(pid, 9);

    } else {
        print_help();
        return 1;
    }
}

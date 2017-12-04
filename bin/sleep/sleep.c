#include <stdio.h>
#include <string.h>
#include <sys/defs.h>
#include <sys/utility.h>
#include <unistd.h>

/* TODO: Use sys/utility.h char_array_to_int function instead of redefining. For
 * some weird reasons not able to use even though the headfile is included */

uint32_t
charArrayToInt(char* array)
{
    uint32_t number = 0;
    int mult = 1;
    int n = strlen(array);
    while (n--) {
        if ((array[n] < '0' || array[n] > '9') && array[n] != '-') {
            if (number)
                break;
            else
                continue;
        } else {
            number += (array[n] - '0') * mult;
            mult *= 10;
        }
    }
    /* To Test the functionality independently */
    // write(1,argv[1],strlen(argv[1]));
    return number;
}

int
main(int argc, char** argv)
{
    uint32_t seconds;
    if (argc > 1) {
        seconds = charArrayToInt(argv[1]);
        sleep(seconds * 1000);
    } else {
        puts("sleep: missing operand");
    }
    // write(1, argv[1], strlen(argv[1]));
}

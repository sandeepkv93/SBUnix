#include <sys/defs.h>
#include <sys/string.h>
#include <sys/utility.h>

uint64_t
power(uint64_t x, int e)
{
    if (e == 0)
        return 1;

    return x * power(x, e - 1);
}

uint64_t
octal_to_decimal(uint64_t octal)
{
    uint64_t decimal = 0, i = 0;
    while (octal != 0) {
        decimal = decimal + (octal % 10) * power(8, i++);
        octal = octal / 10;
    }
    return decimal;
}

uint64_t
char_array_to_int(char* array)
{
    uint64_t number = 0;
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
    return number;
}
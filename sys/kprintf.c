#include <stdarg.h>
#include <string.h>
#include <sys/kprintf.h>

char* vc = (char*)0xb8000;

int
strlen(const char* s1)
{
    int len = 0;
    while (*s1++ != '\0') {
        len++;
    }
    return len;
}

char*
strrev(char* str)
{
    char *p1, *p2;

    if (!str || !*str)
        return str;
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}

void
copy_hex(unsigned long num, char* buf, int* ptr)
{
    char hex_buffer[64] = { '\0' };
    char hex_map[6] = { 'a', 'b', 'c', 'd', 'e', 'f' };
    int i = 0;
    long temp;
    if (!num) {
        hex_buffer[i++] = '0';
    }
    while (num) {
        temp = num % 16;
        hex_buffer[i++] = (temp < 10) ? (char)(temp + '0') : hex_map[temp - 10];
        num = num / 16;
    }
    strrev(hex_buffer);
    i = 0;
    while (i < strlen(hex_buffer)) {
        buf[*ptr] = hex_buffer[i++];
        *ptr += 1;
    }
}

void
copy_integer(int i, char* buf, int* ptr)
{
    int d;
    int k = 0;
    char int_buffer[64];
    while (i > 0) {
        d = i % 10;
        i /= 10;
        int_buffer[k++] = (char)(d + 48);
    }
    strrev(int_buffer);
    k = 0;
    while (k < strlen(int_buffer)) {
        buf[*ptr] = int_buffer[k++];
        *ptr += 1;
    }
}
void
signalme(char c)
{
    char* t;
    t = (char*)0xb8000;
    t += (160 * 4);
    *t = c;
}

void
print_to_console(const char* buf, int buflen)
{
    int k = 0;
    while (k < buflen) {
        *vc = buf[k++];
        vc += 2;
    }
}

void
kprintf(const char* arg1, ...)
{
    char buffer[4096] = { '\0' };
    int bufptr = 0;
    va_list ap;
    va_start(ap, arg1);
    int len = strlen(arg1);
    int i = 0;
    char* st;
    while (i < len) {
        if (arg1[i] != '%') {
            buffer[bufptr++] = arg1[i++];
            continue;
        }
        ++i;
        switch (arg1[i]) {
            case 'c':
                buffer[bufptr++] = (char)va_arg(ap, int);
                break;
            case 's':
                st = (char*)va_arg(ap, char*);
                while (*st) {
                    buffer[bufptr++] = *st++;
                }
                break;
            case 'd':
                copy_integer(va_arg(ap, int), buffer, &bufptr);
                break;
            case 'x':
                copy_hex(va_arg(ap, unsigned long), buffer, &bufptr);
                break;
            case 'p':
                buffer[bufptr++] = '0';
                buffer[bufptr++] = 'x';
                copy_hex(va_arg(ap, unsigned long), buffer, &bufptr);
                break;
        }
        ++i;
    }
    print_to_console(buffer,bufptr);
    va_end(ap);
}

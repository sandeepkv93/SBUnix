#include <stdarg.h>
#include <sys/interrupts.h>
#include <sys/kprintf.h>
#include <sys/string.h>
#include <sys/term.h>

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
    char int_buffer[64] = { '\0' };
    if (i == 0) {
        buf[*ptr] = '0';
        *ptr += 1;
        return;
    }
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

int
vprintf(char* buffer, const char* format, va_list ap)
{
    int len = strlen(format);
    int i = 0;
    int bufptr = 0;
    char* st;
    while (i < len) {
        if (format[i] != '%') {
            buffer[bufptr++] = format[i++];
            continue;
        }
        ++i;
        switch (format[i]) {
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
    buffer[bufptr] = '\0';
    return bufptr;
}
int
sprintf(char* buffer, const char* format, ...)
{
    int buflen = 0;
    va_list ap;
    va_start(ap, format);
    buflen = vprintf(buffer, format, ap);
    va_end(ap);
    return buflen;
}

void
kprintf(const char* format, ...)
{
    // TODO Don't allocate big chunks on stack. Use kmalloc
    char buffer[1024] = { '\0' };
    int buflen = 0;
    va_list ap;
    va_start(ap, format);
    buflen = vprintf(buffer, format, ap);
    term_write(buffer, buflen);
    va_end(ap);
}

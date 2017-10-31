#include <stdarg.h>
#include <sys/interrupts.h>
#include <sys/kprintf.h>
#include <sys/string.h>
#define VC_ROW_LIMIT 25
#define VC_COL_LIMIT 80
#define VIDEO_MEMORY 0xffffffff80800000

char* vc = (char*)VIDEO_MEMORY;
int vc_col = 0;
int vc_row = 0;

void
initialize_vc_memory()
{
    vc = (char*)VIDEO_MEMORY;
    vc_col = 0;
    vc_row = 0;
}

int
check_if_buffer_overflow()
{
    if (vc_row >= VC_ROW_LIMIT) {
        return 1;
    } else {
        return 0;
    }
}

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
void
signalme(char c)
{
    char* t;
    t = (char*)VIDEO_MEMORY;
    t += (160 * 4);
    *t = c;
}

void
print_to_console(const char* buf, int buflen)
{
    int k = 0;
    while (k < buflen) {
        vc_col += 1;
        if (vc_col >= VC_COL_LIMIT) {
            vc_row += 1;
            vc_col = 0;
        }
        if (buf[k] == '\n') {
            vc_row += 1;
            vc_col = 0;
            vc = ((char*)(VIDEO_MEMORY) + (vc_row * 2 * VC_COL_LIMIT));
            ++k;
            continue;
        }
        if (buf[k] == '\r') {
            vc_col = 0;
            vc = ((char*)(VIDEO_MEMORY) + (vc_row * 2 * VC_COL_LIMIT));
            ++k;
            continue;
        }
        *vc = buf[k++];
        vc += 2;
        if (check_if_buffer_overflow()) {
            initialize_vc_memory();
        }
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
    int bufptr = 0;
    va_list ap;
    va_start(ap, format);
    bufptr = vprintf(buffer, format, ap);
    /*    int len = strlen(format);
        int i = 0;
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
    */
    va_end(ap);
    return bufptr;
}

void
kprintf(const char* format, ...)
{
    char buffer[4096] = { '\0' };
    int bufptr = 0;
    va_list ap;
    va_start(ap, format);
    bufptr = vprintf(buffer, format, ap);
    /*
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
    */
    print_to_console(buffer, bufptr);
    va_end(ap);
}

void
get_cursor_position(int* row, int* col)
{
    *row = vc_row;
    *col = vc_col;
}

void
cursor_move(int row, int col)
{
    vc_row = row;
    vc_col = col;
    vc = ((char*)(VIDEO_MEMORY) + (vc_row * 160 + vc_col * 2));
}

void
clear_screen()
{
    initialize_vc_memory();
    int i;
    for (i = 0; i < 25 * 80; i++) {
        kprintf(" ");
    }
    initialize_vc_memory();
}

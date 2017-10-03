#ifndef __KPRINTF_H
#define __KPRINTF_H

void kprintf(const char*, ...);
void signalme(char);
void clear_screen();
void cursor_move(int, int);
void get_cursor_position(int* row, int* col);
int sprintf(char*, const char*, ...);
#endif

#ifndef _TERM_H
#define _TERM_H
#define VIDEO_MEMORY 0xffffffff80800000
void term_write(const char* buf, int buflen);
void term_clear_screen();
void term_set_cursor(int row, int column);
void term_get_cursor(int* row, int* col);
#endif

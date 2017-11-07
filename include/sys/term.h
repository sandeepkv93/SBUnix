#ifndef _TERM_H
#define _TERM_H
#define VIDEO_MEMORY 0xffffffff80800000
#define TERM_BG_FG_COLOR(x, y) ((x << 4) | y)
enum term_colors
{
    term_color_black = 0x0,
    term_color_blue,
    term_color_green,
    term_color_cyan,
    term_color_red,
    term_color_megenta,
    term_color_brown,
    term_color_grey,
    term_color_darkgrey,
    term_color_lightblue,
    term_color_lightgreen,
    term_color_lightcyan,
    term_color_lightred,
    term_color_lightmegenta,
    term_color_yellow,
    term_color_white
};

void term_write(const char* buf, int buflen);
void term_clear_screen();
void term_set_cursor(uint8_t row, uint8_t column, uint8_t color);
void term_get_cursor(int* row, int* col);
#endif

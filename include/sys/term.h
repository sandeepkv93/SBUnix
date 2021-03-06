#ifndef _TERM_H
#define _TERM_H
#define TERM_BG_FG_COLOR(x, y) ((x << 4) | y)
#define TERM_VFS_OBJ 0xdead0000
#define TERM_VFS_MASK 0xffff0000
#define TERM_READ_OBJ (TERM_VFS_OBJ | STDIN)
#define TERM_WRITE_OBJ (TERM_VFS_OBJ | STDOUT)
#define TERM_ERROR_OBJ (TERM_VFS_OBJ | STDERR)

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

void term_set_time(uint64_t seconds);
void term_set_keypress(uint8_t code, uint8_t is_ctrl_pressed,
                       uint8_t is_shift_pressed);
int term_write(const char* buf, int buflen);
void term_clear_screen();
void term_set_cursor(uint8_t row, uint8_t column, uint8_t color);
void term_get_cursor(uint8_t* row, uint8_t* column, uint8_t* color);
void term_set_glyph(uint8_t pos, char c);
int term_read_from_buffer(char* buff, int count);
#endif

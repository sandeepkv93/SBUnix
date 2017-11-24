#include <sys/interrupts.h>
#include <sys/kprintf.h>
#include <sys/term.h>
#define VC_ROW_LIMIT 25
#define VC_COL_LIMIT 80
#define VC_CHAR_PER_COL 2
#define TERM_DEFAULT_COLOR 0x07
#define TERM_KEY_BUFFER_SIZE 1024

extern char g_keymap[], g_keymap_shift[];

// This maintains the video cursor
volatile struct
{
    uint8_t row;
    uint8_t column;
    uint8_t color;
} v_cursor = {.row = 1, .column = 0, .color = TERM_DEFAULT_COLOR };

// This maintains the keyboard buffer
volatile struct
{
    char buffer[TERM_KEY_BUFFER_SIZE];
    uint16_t end;
    bool block;
    bool lock;
} kb_buff = {.block = TRUE };

void
term_scroll_on_overflow()
{
    char* v_mem = (char*)TERM_VIDEO_MEMORY;
    if (v_cursor.row >= VC_ROW_LIMIT) {
        /* If we just need to overwrite use following. Else we'll scroll
        v_cursor.row = 0;
        v_cursor.column = 0;*/

        // We don't scroll the first line. Hence start from VC_COL_LIMIT
        for (int j = VC_CHAR_PER_COL * VC_COL_LIMIT;
             j < VC_ROW_LIMIT * VC_COL_LIMIT * VC_CHAR_PER_COL; j++) {
            if (j >= (VC_ROW_LIMIT - 1) * VC_COL_LIMIT * VC_CHAR_PER_COL) {
                // This writes a blank line in the end
                v_mem[j] = ' ';
                j++;
            } else {
                // Reset of the lines are copies of the next line
                v_mem[j] = v_mem[j + VC_COL_LIMIT * VC_CHAR_PER_COL];
            }
        }
        // Cursor set to last line
        v_cursor.row = VC_ROW_LIMIT - 1;
        v_cursor.column = 0;
    }
}

int
term_write(const char* buf, int buflen)
{
    uint32_t index = 0;
    char* v_mem = (char*)TERM_VIDEO_MEMORY;
    bool interrupts_enabled = are_interrupts_enabled();

    /*
     TODO Handle this interrupt thing gracefully. This hack is needed for the
     following: An interrupt occurs in whose handler we do a kprintf. Now if
     this interrupt occurs again before kprintf finishes we are in trouble
     because the things to kprintf is ever increasing.

     Solution is to have a properly implemented top half and bottom half. We
     could dedicate a kernel thread to that.
     */

    if (interrupts_enabled) {
        enable_interrupts(FALSE);
    }

    for (int i = 0; i < buflen; i++) {

        // Handle newline and carriage return
        if (buf[i] == '\n') {
            v_cursor.row++;
            v_cursor.column = 0;
            term_scroll_on_overflow();
            continue;
        } else if (buf[i] == '\r') {
            v_cursor.column = 0;
            continue;
        }

        // Column overflows into next row
        if (v_cursor.column >= VC_COL_LIMIT) {
            v_cursor.row++;
            v_cursor.column = 0;
        }

        term_scroll_on_overflow();
        // Compute index to write from cursor location. Bad?
        index = (v_cursor.row * VC_COL_LIMIT * VC_CHAR_PER_COL) +
                (v_cursor.column * 2);

        v_mem[index] = buf[i];
        v_mem[index + 1] = v_cursor.color;

        v_cursor.column++;
    }

    if (interrupts_enabled) {
        enable_interrupts(TRUE);
    }

    return buflen;
}

void
term_get_cursor(uint8_t* row, uint8_t* column, uint8_t* color)
{
    *row = v_cursor.row;
    *column = v_cursor.column;
    *color = v_cursor.color;
}

void
term_set_cursor(uint8_t row, uint8_t column, uint8_t color)
{
    v_cursor.row = row;
    v_cursor.column = column;
    v_cursor.color = color;
}

void
term_clear_screen()
{
    char* vc = (char*)TERM_VIDEO_MEMORY;
    for (int i = 0; i < VC_ROW_LIMIT * VC_COL_LIMIT * VC_CHAR_PER_COL; i++) {
        *vc = ' ';
        vc += 2;
    }
    term_set_cursor(1, 0, TERM_DEFAULT_COLOR);
}

void
term_mutex_lock()
{
    // TODO Turn this poor man's mutex into generic implementation

    // TODO disable_context_switching
    while (kb_buff.lock)
        ;
    kb_buff.lock = TRUE;

    // TODO enable_context_switching
    return;
}

void
term_mutex_unlock()
{
    kb_buff.lock = FALSE;
}

void
term_add_to_buffer(char* str, int len)
{
    term_mutex_lock();
    if (kb_buff.end + len - 1 >= TERM_KEY_BUFFER_SIZE) {
        // drop
        term_mutex_unlock();
        return;
    }
    // Local echo
    term_write(str, len);
    for (int i = 0; i < len; i++) {
        kb_buff.buffer[kb_buff.end++] = str[i];
    }
    term_mutex_unlock();
}

void
term_buffer_reset(int pos)
{
    int k = 0;

    for (int i = pos; i < TERM_KEY_BUFFER_SIZE; i++) {
        k = i - pos;
        kb_buff.buffer[k] = kb_buff.buffer[i];
    }

    k++;

    for (; k < TERM_KEY_BUFFER_SIZE; k++) {
        kb_buff.buffer[k] = 0;
    }
}

int
term_read_from_buffer(char* buff, int count)
{
    int i;
    for (i = 0; i < count; i++) {
        if (i >= kb_buff.end) {
            // If user is yet to type we need to wait
            kb_buff.block = TRUE;
        }
        // Wait until atleast one \n is there in buffer
        while (kb_buff.block)
            ;
        if (kb_buff.buffer[i] == '\n') {
            term_buffer_reset(i + 1);
            term_mutex_unlock();
            return i;
        }
        buff[i] = kb_buff.buffer[i];
    }
    term_mutex_lock();
    term_buffer_reset(i + 1);
    term_mutex_unlock();
    return i;
}

void
term_set_keypress(uint8_t code, uint8_t is_ctrl_pressed,
                  uint8_t is_shift_pressed)
{
    char str[2];
    char key = is_shift_pressed ? g_keymap_shift[code] : g_keymap[code];
    char prefix = is_ctrl_pressed ? '^' : key;
    str[0] = prefix;
    str[1] = key;
    term_add_to_buffer(str, is_ctrl_pressed + 1);
    if (prefix == '\n') {
        kb_buff.block = FALSE;
    }
}

void
term_set_time(uint64_t seconds)
{
    // Prints time in the top line. Again coding ugly :(
    char str[20];
    uint8_t strlen;

    uint8_t* vc = (uint8_t*)TERM_VIDEO_MEMORY;
    uint8_t start_position = 116;

    int mins = seconds / 60;
    int secs = seconds % 60;

    strlen = sprintf(str, " Time since boot %d%s%d", mins,
                     secs < 10 ? ":0" : ":", secs);

    for (int i = 0; i < strlen; i++) {
        vc[2 * i + start_position] = str[i];
        vc[2 * i + start_position + 1] =
          TERM_BG_FG_COLOR(term_color_lightgreen, term_color_black);
    }
}
void
term_set_glyph(uint8_t pos, char c)
{
    // This prints a character in column 0 of top line
    char* t;
    t = (char*)TERM_VIDEO_MEMORY + pos * 2;
    *t = c;
}
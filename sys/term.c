#include <sys/interrupts.h>
#include <sys/term.h>
#define VC_ROW_LIMIT 25
#define VC_COL_LIMIT 80
#define VC_CHAR_PER_COL 2

struct
{
    uint8_t row;
    uint8_t column;
    uint8_t color;
} v_cursor = { 1, 0, 0 };

void
term_write(const char* buf, int buflen)
{
    uint32_t index = 0;
    char* v_mem = (char*)VIDEO_MEMORY;

    /*
     TODO Handle this interrupt thing gracefully. This hack is needed for the
     following: An interrupt occurs in whose handler we do a kprintf. Now if
     this interrupt occurs again before kprintf finishes we are in trouble
     because the things to kprintf is ever increasing.

     Solution is to have a properly implemented top half and bottom half. We
     could dedicate a kernel thread to that.
     */

    enable_interrupts(FALSE);
    for (int i = 0; i < buflen; i++) {

        // Handle newline and carriage return
        if (buf[i] == '\n') {
            v_cursor.row++;
            v_cursor.column = 0;
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

        // Row overflows should scroll
        if (v_cursor.row >= VC_ROW_LIMIT) {
            /* If we just need to overwrite use following. Else we'll scroll
            v_cursor.row = 0;
            v_cursor.column = 0;*/

            // We don't scroll the first line. Hence start from VC_COL_LIMIT
            for (int j = VC_COL_LIMIT;
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

        // Compute index to write from cursor location. Bad?
        index = (v_cursor.row * VC_COL_LIMIT * VC_CHAR_PER_COL) +
                (v_cursor.column * 2);

        v_mem[index] = buf[i];

        v_cursor.column++;
    }
    enable_interrupts(TRUE);
}

void
term_get_cursor(int* row, int* col)
{
    *row = v_cursor.row;
    *col = v_cursor.column;
}

void
term_set_cursor(int row, int column)
{
    v_cursor.row = row;
    v_cursor.column = column;
}

void
term_clear_screen()
{
    char* vc = (char*)VIDEO_MEMORY;
    for (int i = 0; i < VC_ROW_LIMIT * VC_COL_LIMIT * VC_CHAR_PER_COL; i++) {
        *vc = ' ';
        vc += 2;
    }
    term_set_cursor(1, 0);
}

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYCODE_RELEASE 0x80
#define KEYCODE_SHIFT 0x36
#define KEYCODE_LSHIFT 0x2a
#define KEYCODE_LCTRL 0x1d

// Refer https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
char g_keymap[] = {
    83,  // Use this to denote size of array
    ' ', // Escape 0x01
    '1',  '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',  '=',
    ' ', // Backspace 0x0e
    ' ', // Tab
    'q',  'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',  ']',
    '\n', // Enter
    ' ',  // LCtrl 0x1d
    'a',  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    ' ', // Shift
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    ' ', // Rshift 0x36
    ' ', // Keypad* or PrtSc
    ' ', // Left Alt
    ' ', // Spacebar
    ' ', // Capslock
    ' ', // F1
    ' ', // F2
    ' ', // F3
    ' ', // F4
    ' ', // F5
    ' ', // F6
    ' ', // F7
    ' ', // F8
    ' ', // F9
    ' ', // F10
    ' ', // Numlock
    ' ', // Scroll Lock
    '7',  '8', '9', '-', '4', '5', '6', '+', '1', '2', '3',  '0', '.'
};

char g_keymap_shift[] = {
    83,
    ' ', // Escape
    '!',  '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
    ' ', // Backspace
    ' ', // Tab
    'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
    '\n', // Enter
    ' ',  // LCtrl
    'A',  'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    ' ', //
    '|',  'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    ' ', // Rshift
    ' ', //
    ' ', // Left Alt
    ' ', // Spacebar
    ' ', // Capslock
    ' ', // F1
    ' ', // F2
    ' ', // F3
    ' ', // F4
    ' ', // F5
    ' ', // F6
    ' ', // F7
    ' ', // F8
    ' ', // F9
    ' ', // F10
    ' ', // Numlock
    ' ', // Scroll Lock
    '7',  '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
};
#endif

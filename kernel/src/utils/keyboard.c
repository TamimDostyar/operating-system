#include "keyboard.h"
#include "inb.h"

char ascii_normal[256] = {0};
static char keyboard_buffer[256];
static int buffer_head = 0;
static int buffer_tail = 0;


/*
    - For keyboard we use scancodes to capture what was pressed
    - so basically we need input and output for this
*/

void keyboard_init(void) {

    // all keys
    ascii_normal[scanA] = 'a';
    ascii_normal[scanB] = 'b';
    ascii_normal[scanC] = 'c';
    ascii_normal[scanD] = 'd';
    ascii_normal[scanE] = 'e';
    ascii_normal[scanF] = 'f';
    ascii_normal[scanG] = 'g';
    ascii_normal[scanH] = 'h';
    ascii_normal[scanI] = 'i';
    ascii_normal[scanJ] = 'j';
    ascii_normal[scanK] = 'k';
    ascii_normal[scanL] = 'l';
    ascii_normal[scanM] = 'm';
    ascii_normal[scanN] = 'n';
    ascii_normal[scanO] = 'o';
    ascii_normal[scanP] = 'p';
    ascii_normal[scanQ] = 'q';
    ascii_normal[scanR] = 'r';
    ascii_normal[scanS] = 's';
    ascii_normal[scanT] = 't';
    ascii_normal[scanU] = 'u';
    ascii_normal[scanV] = 'v';
    ascii_normal[scanW] = 'w';
    ascii_normal[scanX] = 'x';
    ascii_normal[scanY] = 'y';
    ascii_normal[scanZ] = 'z';

    // punctuations
    ascii_normal[scanSemicolon] = ';';
    ascii_normal[scanQuote] = '\'';
    ascii_normal[scanComma] = ',';
    ascii_normal[scanDot] = '.';
    ascii_normal[scanSlash] = '/';
    ascii_normal[scanBackQuote] = '`';
    
    // special cases
    ascii_normal[scanEnter]     = '\n';
    ascii_normal[scanBackspace] = '\b';
    ascii_normal[scanSpace] = ' ';

}

void keyboardHandler(void) {
    uint8_t scancode = inb(0x60);
    if (scancode & 0x80)    /* ignore key-release events */
        return;
    char c = ascii_normal[scancode];
    keyboard_buffer[buffer_head] = c;
    buffer_head = (buffer_head + 1) % 256;
}

char keyboard_getchar(void) {
    if (buffer_head == buffer_tail)
        return 0;
    char c = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % 256;
    return c;
}
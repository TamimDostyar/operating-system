#include "keyboard.h"
#include "inb.h"

char ascii_normal[256] = {0};
static char keyboard_buffer[256];
static int buffer_head = 0;
static int buffer_tail = 0;

// boolean if shift/capslock is pressed to capitalize the charracters
static int isShiftPressed = 0;
static int capslock_on = 0;
/*
    - For keyboard we use scancodes to capture what was pressed
    - so basically we need input and output for this
*/

int keyboard_init(void) {
    // numbers

    ascii_normal[scanEqual] = '=';
    ascii_normal[scanMinus] = '-';
    ascii_normal[scan0] = '0';
    ascii_normal[scan1] = '1';
    ascii_normal[scan2] = '2';
    ascii_normal[scan3] = '3';
    ascii_normal[scan4] = '4';
    ascii_normal[scan5] = '5';
    ascii_normal[scan6] = '6';
    ascii_normal[scan7] = '7';
    ascii_normal[scan8] = '8';
    ascii_normal[scan9] = '9';
    // brackets
    ascii_normal[scanLeftBracket] = '[';
    ascii_normal[scanRightBracket] = ']';

    // punctuations
    ascii_normal[scanSemicolon] = ';';
    ascii_normal[scanQuote] = '\'';
    ascii_normal[scanComma] = ',';
    ascii_normal[scanDot] = '.';
    ascii_normal[scanSlash] = '/';
    ascii_normal[scanBackQuote] = '`';
    ascii_normal[scanBackSlash] = '\\';
    
    // special cases
    ascii_normal[scanEnter]     = '\n';
    ascii_normal[scanBackspace] = '\b';
    ascii_normal[scanSpace] = ' ';

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

    return 1;
}

void keyboardHandler(void) {

    // 0x60 data port
    //0x80 key release bit
    // 0x7F bitmask to strip the release bit
    uint8_t scancode = inb(0x60);


    if (scancode & 0x80) {
        uint8_t released = scancode & 0x7F;
        // if shift is released
        if (released == scanLeftShift || released == scanRightShift)
            isShiftPressed = 0;
        return;
    }

    if (scancode == scanCapsLock) {
        capslock_on = !capslock_on;
        return;
    }

    if (scancode == scanLeftShift || scancode == scanRightShift) {
        isShiftPressed = 1;
        return;
    }

    char c = ascii_normal[scancode];

    if (c >= 'a' && c <= 'z') {
        /* 
         XOR ^ --> true only when exactly one of the two is active.
         comparing bitwise!
         * Shift ON + CapsLock OFF → uppercase
         * Shift OFF + CapsLock ON → uppercase
         * Both ON or both OFF      → lowercase
         * ASCII trick: subtracting 'a' gives the 0-based letter index,
         * then adding 'A' maps it into the uppercase range. 
         */
        if (isShiftPressed ^ capslock_on)
            c = c - 'a' + 'A';
        
    }

    if (isShiftPressed) {
        switch (scancode) {
            case scan1: c = '!'; break;
            case scan2: c = '@'; break;
            case scan3: c = '#'; break;
            case scan4: c = '$'; break;
            case scan5: c = '%'; break;
            case scan6: c = '^'; break;
            case scan7: c = '&'; break;
            case scan8: c = '*'; break;
            case scan9: c = '('; break;
            case scan0: c = ')'; break;
            case scanMinus: c = '_'; break;
            case scanEqual: c = '+'; break;
            case scanSlash: c = '?'; break;
            case scanLeftBracket: c = '{'; break;
            case scanRightBracket: c = '}'; break;
            case scanBackSlash: c = '|'; break;
            case scanBackQuote: c = '~'; break;
            case scanComma: c = '<'; break;
            case scanDot: c = '>'; break;
            
        }
    }

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
#include <stdint.h>
#include "vga.h"


// 0xB8000 physcall memory address for the vga
static uint16_t * const VGA_MEMORY = (uint16_t*)0xB8000;
static int cursor_row = 0;
static int cursor_col = 0;
static uint8_t vga_current_color = 0;

static inline uint8_t vga_make_color(uint8_t fg, uint8_t bg) {
    return (bg << 4) | (fg & 0x0F);
}

//helper get current 16-bit value for a character
static inline uint16_t vga_entry(char c, uint8_t color) {
    return ((uint16_t)color << 8) | (uint8_t)c;
}

void vga_init() {
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear();
}

void vga_clear() {
    for (int row = 0; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            VGA_MEMORY[row * VGA_WIDTH + col] = vga_entry(' ', vga_current_color);
        }
    }
    cursor_row = 0;
    cursor_col = 0;
}

void vga_set_color(uint8_t fg, uint8_t bg) {
    vga_current_color = vga_make_color(fg, bg);
}

static void vga_scroll() {
    for (int row = 1; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            VGA_MEMORY[(row - 1) * VGA_WIDTH + col] = VGA_MEMORY[row * VGA_WIDTH + col];
        }
    }
    for (int col = 0; col < VGA_WIDTH; col++) {
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + col] = vga_entry(' ', vga_current_color);
    }
    cursor_row = VGA_HEIGHT - 1;
    cursor_col = 0;
}

void vga_putchar(char c) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
    } else {
        VGA_MEMORY[cursor_row * VGA_WIDTH + cursor_col] = vga_entry(c, vga_current_color);
        cursor_col++;
        if (cursor_col >= VGA_WIDTH) {
            cursor_col = 0;
            cursor_row++;
        }
    }
    if (cursor_row >= VGA_HEIGHT) {
        vga_scroll();
    }
}

// write in the position of the cursor
void vga_write(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        vga_putchar(str[i]);
    }
}
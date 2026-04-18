#include "../utils/vga.h"
#include "../utils/keyboard.h"
#include "../utils/idt.h"

void kmain(void) {
    vga_init();
    keyboard_init();
    idt_install();

    vga_write("Loading kernel\n");
    vga_write("Kernel is running successfully!\n");
    vga_write("To test the keyboard type characters\n");
    vga_write("\n");

    __asm__ volatile("sti");

    for (;;) {
        char c = keyboard_getchar();
        if (c != 0) {
            vga_putchar(c);
        }
    }
}

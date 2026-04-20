#include "../utils/vga.h"
#include "../utils/keyboard.h"
#include "../utils/idt.h"
#include "heap.h"

void kmain(void) {

    int vgaOutput = vga_init();
    int keyboardOutput = keyboard_init();
    int idtOutput = idt_install();

    // heap
    heap_init();
    int *memory = kmalloc(sizeof(memory));
    kfree(memory);
    
    // set interrupt 
    __asm__ volatile("sti");
    vga_write("Kernell is working and running successfully\n");
    vga_write("\n");
    vga_write("buddyOS> ");
    vga_lock_cursor();


    for (;;) {
        char c = keyboard_getchar();
        if (c != 0) {
            vga_putchar(c);
        }
    }
}

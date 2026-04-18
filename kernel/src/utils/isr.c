#include "idt.h"
#include "vga.h"
#include "keyboard.h"
#include "inb.h"

#define PIC1_CMD 0x20
#define PIC2_CMD 0xA0
#define PIC_EOI  0x20

struct regs {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

void isr_handler(struct regs *r) {
    (void)r;
    vga_write("CPU exception!\n");
}

void irq_handler(struct regs *r) {
    if (r->int_no == 33)    /* IRQ1: keyboard */
        keyboardHandler();

    if (r->int_no >= 40)
        outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}

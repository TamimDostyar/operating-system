#include "idt.h"
#include <stdio.h>
struct regs {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

void isr_handler(struct regs *r) {
    if (r->int_no < 32) {
        // CPU exception
        printf("Exception: %d\n", r->int_no);
        // Handle page fault, divide by zero, etc.
    }
    // Handle hardware interrupts
}
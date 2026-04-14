#include "idt.h"

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void isr0();
extern void isr1();

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void idt_install() {
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)&idt;

    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);

    asm volatile("lidt (%0)" : : "r" (&idtp));
}
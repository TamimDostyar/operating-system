#include <stdint.h>

struct idt_entry {
    uint16_t base_low;      // Lower 16 bits of handler function address
    uint16_t sel;               // Kernel segment selector
    uint8_t  always0;    // Must be zero
    uint8_t  flags;      // type, DPL, present
    uint16_t base_high;  // Upper 16 bits of handler address
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;      // Size of IDT
    uint32_t base;       // Base address of IDT
} __attribute__((packed));


void idt_install();
void process_init();
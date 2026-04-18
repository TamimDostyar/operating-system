# Kernel

The kernel is the core of BuddyOS. It runs in 32-bit protected mode after the bootloader hands over control. It sets up the screen, keyboard, and interrupts, then sits in a loop printing whatever you type.

---

## File Overview

```
kernel/
└── src/
    ├── manager/
    │   └── kmain.c          ← kernel entry point
    └── utils/
        ├── vga.h / vga.c    ← screen output
        ├── keyboard.h / keyboard.c  ← keyboard input
        ├── idt.h / idt.c    ← interrupt table setup
        ├── isr.c            ← interrupt handlers
        └── inb.h            ← hardware port read/write
```

---

## `manager/kmain.c`

The first C function that runs. It calls the setup functions in order, prints startup messages, enables interrupts, then loops forever reading keypresses and printing them.

| Function | What it does |
|----------|-------------|
| `kmain()` | Initializes everything and runs the main keyboard loop |

---

## `utils/vga.h` / `utils/vga.c`

Handles writing text to the screen. VGA text mode lets you print characters by writing directly to memory address `0xB8000`. The screen is 80 columns × 25 rows.

| Function | What it does |
|----------|-------------|
| `vga_init()` | Clears the screen and sets the default color (green on black) |
| `vga_clear()` | Fills every cell on screen with a blank space |
| `vga_set_color(fg, bg)` | Changes the text and background color |
| `vga_putchar(c)` | Prints one character; handles `\n` (newline), `\b` (backspace), and line wrapping |
| `vga_write(str)` | Prints a whole string by calling `vga_putchar` for each character |

---

## `utils/keyboard.h` / `utils/keyboard.c`

Handles reading keys from the keyboard. The keyboard sends a **scancode** (a number identifying the physical key) over hardware port `0x60`. This driver translates scancodes into ASCII characters and stores them in a buffer.

| Function | What it does |
|----------|-------------|
| `keyboard_init()` | Builds the scancode-to-ASCII lookup table |
| `keyboardHandler()` | Called by the interrupt system when a key is pressed; reads the scancode and adds the character to the buffer |
| `keyboard_getchar()` | Returns the next character from the buffer; returns `0` if nothing has been typed |

---

## `utils/idt.h` / `utils/idt.c`

Sets up the **Interrupt Descriptor Table (IDT)** — a table that tells the CPU which function to call when an interrupt fires (keyboard press, CPU error, etc.). Also reprograms the **PIC** (Programmable Interrupt Controller) so hardware interrupts don't collide with CPU error codes.

| Function | What it does |
|----------|-------------|
| `idt_set_gate(num, base, sel, flags)` | Registers one interrupt handler in the IDT at slot `num` |
| `idt_install()` | Fills all 256 IDT slots, remaps the PIC, and loads the IDT into the CPU |
| `pic_remap()` *(internal)* | Reprograms the PIC so IRQs map to interrupts 32–47 instead of 0–15 |

---

## `utils/isr.c`

Contains the actual C functions that run when an interrupt fires. The ISR stubs in `boot/isr_stubs.s` save CPU state and then call these.

| Function | What it does |
|----------|-------------|
| `isr_handler(r)` | Called on any CPU exception (divide by zero, bad memory access, etc.); currently prints "CPU exception!" |
| `irq_handler(r)` | Called on hardware interrupts; routes keyboard interrupts to `keyboardHandler()` and sends End-of-Interrupt to the PIC |

---

## `utils/inb.h`

Tiny helpers for reading and writing hardware I/O ports using x86 `in`/`out` instructions. Used by the keyboard driver (port `0x60`) and the PIC setup (ports `0x20`, `0xA0`).

| Function | What it does |
|----------|-------------|
| `inb(port)` | Reads 1 byte from a hardware port |
| `outb(port, data)` | Writes 1 byte to a hardware port |
| `inw(port)` | Reads 2 bytes from a hardware port |
| `outw(port, data)` | Writes 2 bytes to a hardware port |

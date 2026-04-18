# BuddyOS — Complete Beginner's Guide to the Kernel

---

## Table of Contents

1. [Big Picture — How a PC Boots](#1-big-picture--how-a-pc-boots)
2. [Memory Map — Where Things Live in RAM](#2-memory-map--where-things-live-in-ram)
3. [Build System — `Makefile`](#3-build-system--makefile)
4. [GDT — `boot/gdt.asm`](#4-gdt--bootgdtasm)
5. [Bootloader — `boot/boot.asm`](#5-bootloader--bootbootasm)
6. [Kernel Entry — `boot/entry.asm`](#6-kernel-entry--bootentryasm)
7. [ISR Stubs — `boot/isr_stubs.s`](#7-isr-stubs--bootisr_stubss)
8. [IDT Header — `kernel/src/utils/idt.h`](#8-idt-header--kernelsrcutilsidth)
9. [IDT Implementation — `kernel/src/utils/idt.c`](#9-idt-implementation--kernelsrcutilsidtc)
10. [ISR/IRQ Handlers — `kernel/src/utils/isr.c`](#10-isrirq-handlers--kernelsrcutilsisrc)
11. [I/O Port Helpers — `kernel/src/utils/inb.h`](#11-io-port-helpers--kernelsrcutilsinbh)
12. [VGA Driver Header — `kernel/src/utils/vga.h`](#12-vga-driver-header--kernelsrcutilsvgah)
13. [VGA Driver — `kernel/src/utils/vga.c`](#13-vga-driver--kernelsrcutilsvgac)
14. [Keyboard Driver Header — `kernel/src/utils/keyboard.h`](#14-keyboard-driver-header--kernelsrcutilskeyboardh)
15. [Keyboard Driver — `kernel/src/utils/keyboard.c`](#15-keyboard-driver--kernelsrcutilskeyboardc)
16. [Kernel Main — `kernel/src/manager/kmain.c`](#16-kernel-main--kernelsrcmanagerkmainc)
17. [Full Boot-to-Keypress Flow](#17-full-boot-to-keypress-flow)
18. [Quick-Reference: Every Hex Value in the Codebase](#18-quick-reference-every-hex-value-in-the-codebase)

---

## 1. Big Picture — How a PC Boots

When you press the power button on a computer, a tiny program burned into a chip called the **BIOS** (Basic Input/Output System) wakes up. The BIOS does some hardware checks and then looks for a **bootable disk**.

It reads the very first 512 bytes of the disk into memory at address **0x7C00** and then jumps to that address. Those 512 bytes are called the **boot sector** (or bootloader). That is `boot/boot.asm`.

The bootloader's job is simple:
1. Load the real kernel from disk into RAM.
2. Set up a thing called the **GDT**.
3. Switch the CPU from 16-bit "real mode" into 32-bit "protected mode".
4. Hand control to the kernel.

### Real Mode vs. Protected Mode

| | Real Mode | Protected Mode |
|---|---|---|
| Bit width | 16-bit | 32-bit |
| Max RAM | 1 MB | 4 GB |
| Memory protection | None | Segments/paging |
| Used by | BIOS & bootloader | The kernel |

The CPU starts in **real mode** because that is what the BIOS expects. The kernel needs **protected mode** to access more memory and to set up proper isolation.

---

## 2. Memory Map — Where Things Live in RAM

During boot the key addresses are:

```
0x00000000  ──── start of RAM
0x00000500  ──── free low memory (BIOS data area ends here)
0x00007C00  ──── bootloader is loaded HERE by BIOS  ← boot.asm starts
0x00007E00  ──── free area after boot sector
0x00001000  ──── kernel is loaded HERE by bootloader ← entry.asm / kmain
0x0009C000  ──── stack top (grows downward from here)
0x000A0000  ──── start of BIOS video memory
0x000B8000  ──── VGA text mode buffer            ← vga.c writes here
0x000FFFFF  ──── top of first megabyte
```

> **Why 0x7C00?** That is just a historical decision by early IBM engineers. The BIOS has always put the boot sector there.

> **Why 0xB8000?** That physical address is wired directly to your monitor's text circuitry. Writing a character there makes it appear on screen immediately — no operating system needed.

---

## 3. Build System — `Makefile`

A `Makefile` is a recipe file. You type `make` and it figures out what to compile, in what order, and with what flags. Here is the whole file explained.

### Variables (lines 1–29)

```makefile
CC       ?= cc
```
`CC` is the C compiler for **userspace** (the shell program). The `?=` means "use this default only if `CC` is not already set in the environment".

```makefile
CFLAGS   ?= -Wall -Wextra -std=c11 -g
```
Compiler flags for the shell:
- `-Wall` — turn on all common warnings
- `-Wextra` — turn on extra warnings
- `-std=c11` — use the C11 standard
- `-g` — include debug information

```makefile
NASM     ?= nasm
```
`NASM` is the assembler used for `.asm` files (the Intel-syntax assembler).

```makefile
QEMU     ?= qemu-system-i386
```
`QEMU` is the virtual machine used to run the OS image.

```makefile
CROSS_PREFIX := i686-elf-
CC_KERNEL    := $(CROSS_PREFIX)gcc
LD_KERNEL    := $(CROSS_PREFIX)ld
OBJCOPY      := $(CROSS_PREFIX)objcopy
```
**Why a cross-compiler?** Your laptop probably runs 64-bit macOS or Linux. But the kernel must be compiled for a 32-bit x86 PC with no OS underneath it. A **cross-compiler** is a compiler that runs on your machine but produces code for a *different* machine — in this case, `i686` (32-bit x86 without any OS).

- `i686-elf-gcc` — GCC targeting bare-metal 32-bit x86
- `i686-elf-ld` — linker for the same target
- `i686-elf-objcopy` — converts ELF binary into a raw binary

```makefile
BUILD_DIR      := build
```
All generated files go into `build/`. The source tree is never touched.

### Source file lists (lines 44–57)

```makefile
KERNEL_ENTRY  := $(BOOT_BUILD)/entry.o
```
The kernel's very first object file. It must be linked first so the code lands exactly at address 0x1000.

```makefile
BOOT_ASM_OBJS := $(BOOT_BUILD)/isr_stubs.o
```
The interrupt stub assembly (the small trampolines that save CPU state before calling C handlers).

```makefile
KERNEL_C_SRCS := $(wildcard $(KERNEL_SRC)/manager/*.c $(KERNEL_SRC)/utils/*.c)
```
Finds all `.c` files under `kernel/src/manager/` and `kernel/src/utils/` automatically.

### Build rules (lines 59–101)

```makefile
$(BOOT_BIN): $(BOOT_DIR)/boot.asm | $(BUILD_DIR)
    $(NASM) -f bin $< -o $@
```
Compile `boot.asm` to a **raw binary** (`-f bin` means "flat binary, no header"). The result is exactly 512 bytes — the boot sector.

```makefile
$(BOOT_BUILD)/entry.o: $(BOOT_DIR)/entry.asm | $(BOOT_BUILD)
    $(NASM) -f elf32 $< -o $@
```
Compile `entry.asm` to a 32-bit ELF object file (`-f elf32`). This is not a raw binary — it will be linked with the C code.

```makefile
$(BOOT_BUILD)/isr_stubs.o: $(BOOT_DIR)/isr_stubs.s | $(BOOT_BUILD)
    $(CC_KERNEL) -m32 -c -o $@ $<
```
Compile the ISR stubs with GCC (`-m32` = 32-bit output, `-c` = compile only, do not link).

```makefile
$(KERNEL_BUILD)/%.o: $(KERNEL_SRC)/%.c | $(KERNEL_BUILD)
    $(CC_KERNEL) -m32 -ffreestanding -nostdlib -c -o $@ $<
```
Compile all kernel `.c` files:
- `-m32` — produce 32-bit code
- `-ffreestanding` — do not assume a standard C library exists; no `printf`, no `malloc`
- `-nostdlib` — do not link against any standard library
- `-c` — compile only, do not link yet

```makefile
$(KERNEL_BIN): $(KERNEL_OBJS) | $(BUILD_DIR)
    $(LD_KERNEL) -m elf_i386 -e _start -Ttext 0x1000 -o $(KERNEL_ELF) $^
    $(OBJCOPY) -O binary $(KERNEL_ELF) $@
```
Link all kernel objects together:
- `-m elf_i386` — produce 32-bit x86 ELF
- `-e _start` — the entry point symbol is `_start` (defined in `entry.asm`)
- `-Ttext 0x1000` — place the code section starting at address **0x1000** in memory. This must match where the bootloader loaded the kernel.
- `objcopy -O binary` — strip all ELF headers and produce a raw binary blob.

```makefile
os-image: boot kernel
    cat $(BOOT_BIN) $(KERNEL_BIN) > $(OS_IMAGE)
```
Glue the boot sector (512 bytes) and the kernel binary together into one file. The disk image layout is:

```
[  sector 0  ] [  sector 1  ] [  sector 2  ] ...
[ boot sector] [  kernel   starts here     ]
  512 bytes       loaded by bootloader
```

The padding at the end ensures the image is at least 5120 bytes (10 sectors), which some QEMU versions expect.

---

## 4. GDT — `boot/gdt.asm`

### What is the GDT?

In 32-bit protected mode the CPU uses a **Global Descriptor Table (GDT)** to define memory segments. A segment is a region of memory with an address, a size, and access permissions. The GDT is a small table in RAM; each entry (called a **descriptor**) is exactly **8 bytes**.

Think of the GDT like a list of "access rules":
- Entry 0 → null (required, do nothing)
- Entry 1 → "kernel code lives here, it may be executed"
- Entry 2 → "kernel data lives here, it may be read and written"

When code runs, the CPU checks the current **segment selector** (a number in a register) against the GDT to decide if the access is allowed.

### The null descriptor

```nasm
gdt_start:
    dq 0x0000000000000000
```
`dq` means "define quad-word" (8 bytes). The first entry **must** be all zeros — it is the null descriptor. The CPU will fault if you ever try to use segment selector 0x00. It exists just to catch bugs.

### Code segment descriptor

```nasm
gdt_code:
    dw 0xFFFF    ; limit bits 0-15
    dw 0x0000    ; base  bits 0-15
    db 0x00      ; base  bits 16-23
    db 0x9A      ; access byte
    db 0xCF      ; flags + limit bits 16-19
    db 0x00      ; base  bits 24-31
```

Each descriptor encodes a **base address** (where the segment starts), a **limit** (how big it is), and **flags** (permissions). The fields are spread across the 8 bytes in a non-obvious order — that is an old Intel legacy.

#### Decoded: base and limit

- Base = `0x00` (high) + `0x00` (mid) + `0x0000` (low) = **0x00000000** — segment starts at the very beginning of RAM.
- Limit raw = `0xF` (high nibble from granularity byte) + `0xFFFF` (low word) = **0xFFFFF**. With granularity flag G=1 this means limit is 0xFFFFF × 4KB = **4 GB** — the whole address space.

#### Decoded: access byte `0x9A`

`0x9A` in binary is `1001 1010`.

| Bit | Name | Value | Meaning |
|-----|------|-------|---------|
| 7 | P (Present) | 1 | This descriptor is valid |
| 6–5 | DPL (ring level) | 00 | Ring 0 = kernel, most privileged |
| 4 | S (descriptor type) | 1 | Code/data segment (not a system gate) |
| 3 | E (Executable) | 1 | This is a **code** segment (can be executed) |
| 2 | DC (Direction/Conforming) | 0 | Code only runs at exactly ring 0 |
| 1 | RW (Readable) | 1 | Code segment is also readable |
| 0 | A (Accessed) | 0 | CPU sets this when segment is used |

#### Decoded: granularity byte `0xCF`

`0xCF` in binary is `1100 1111`.

| Bits | Name | Value | Meaning |
|------|------|-------|---------|
| 7 | G (Granularity) | 1 | Limit is in 4 KB pages (multiply by 4096) |
| 6 | D/B (Default size) | 1 | 32-bit segment (use 32-bit addresses) |
| 5 | L (Long mode) | 0 | Not 64-bit mode |
| 4 | AVL | 0 | Available for OS use (unused here) |
| 3–0 | Limit high | 0xF | Upper 4 bits of the limit field |

### Data segment descriptor

```nasm
gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92    ; access byte — different from code!
    db 0xCF
    db 0x00
```

Access byte `0x92` = `1001 0010` in binary. Same as the code descriptor except bit 3 (E) is **0** — this is a **data** segment, not code. It cannot be executed, only read/written.

| Bit 3 | Name | Value | Meaning |
|-------|------|-------|---------|
| 3 | E (Executable) | 0 | **Data** segment |
| 1 | RW (Writable) | 1 | Data segment is writable |

### GDT descriptor pointer

```nasm
gdt_descriptor:
    dw gdt_end - gdt_start - 1   ; size of GDT in bytes, minus 1
    dd gdt_start                  ; 32-bit address of the GDT in memory
```

`lgdt` (Load GDT) needs a 6-byte structure: a 2-byte size and a 4-byte pointer. The size is `size - 1` because the field holds the **last valid byte offset**, not the count.

### Segment selectors

When you write `mov ds, 0x10`, the number `0x10` is a **selector**. The CPU extracts:
- Bits 15–3: index into the GDT (index 2 = `0x10 >> 3 = 2`)
- Bit 2: TI flag — 0 = use GDT, 1 = use LDT
- Bits 1–0: RPL (requested privilege level) — 00 = ring 0

| Selector | Index | Segment |
|----------|-------|---------|
| `0x00` | 0 | Null — never use |
| `0x08` | 1 | Kernel code segment |
| `0x10` | 2 | Kernel data segment |

---

## 5. Bootloader — `boot/boot.asm`

### Line 1–9: File header and origin

```nasm
[bits 16]
[ORG 0x7C00]

jmp start

%include "boot/gdt.asm"
```

- `[bits 16]` — tell NASM to generate 16-bit instructions. The CPU starts in 16-bit real mode.
- `[ORG 0x7C00]` — tell NASM that this code will be loaded at address 0x7C00. Any absolute addresses (labels) will be calculated relative to this base.
- `jmp start` — skip over the GDT data (which is included here as raw bytes) and jump to the actual code.
- `%include "boot/gdt.asm"` — paste the contents of `gdt.asm` here at assembly time. The GDT data sits in the boot sector itself.

### Lines 11–19: CPU and segment setup

```nasm
start:
    cli                     ; Disable interrupts while we set up
    xor ax, ax              ; ax = 0 (XOR of anything with itself = 0)
    mov ds, ax              ; data segment = 0
    mov es, ax              ; extra segment = 0
    mov ss, ax              ; stack segment = 0
    mov sp, 0x7C00          ; stack pointer starts at 0x7C00
    sti                     ; Re-enable interrupts
```

- `cli` — **Clear Interrupt flag**. Disables hardware interrupts. We do not want the CPU to be interrupted while we set up the stack; it could jump to a random handler and crash.
- `xor ax, ax` — the fastest way to set a register to zero. XOR of a value with itself is always 0.
- `mov ds, ax` — segment registers (`ds`, `es`, `ss`) cannot be loaded with a literal number directly; you must go through a general-purpose register first. Setting them all to 0 means "segment base address = 0".
- `mov sp, 0x7C00` — the **stack pointer** starts at 0x7C00. The stack grows **downward** in x86. So the first value pushed will go to address 0x7BFE, then 0x7BFC, etc. The stack cannot overwrite the bootloader because the bootloader is *at* 0x7C00, and pushing always goes below.
- `sti` — **Set Interrupt flag**. Re-enables interrupts. The BIOS needs interrupts enabled for things like the `int 0x10` print calls.

### Lines 24–37: Loading the kernel from disk

```nasm
load_kernel:
    mov ah, 0x02            ; BIOS function: read sectors from drive
    mov al, 8               ; read 8 sectors (8 × 512 = 4096 bytes)
    mov ch, 0               ; cylinder 0 (first track)
    mov cl, 2               ; start at sector 2 (sector 1 is us, the bootloader)
    mov dh, 0               ; head 0
    mov dl, 0x80            ; drive number: 0x80 = first hard disk
    xor bx, bx
    mov es, bx              ; destination segment = 0x0000
    mov bx, 0x1000          ; destination offset  = 0x1000
    int 0x13                ; call BIOS disk interrupt
    jc disk_error           ; jump if carry flag set = error
```

This is a **BIOS interrupt call**. The BIOS provides a library of small routines accessible by triggering software interrupts with the `int` instruction. `int 0x13` is the disk service.

**Register setup for INT 0x13, AH=0x02:**

| Register | Value | Meaning |
|----------|-------|---------|
| `AH` | `0x02` | Function code: read sectors |
| `AL` | `8` | Number of sectors to read |
| `CH` | `0` | Cylinder number (0-indexed) |
| `CL` | `2` | Sector number (1-indexed! sector 1 = bootloader, sector 2 = kernel) |
| `DH` | `0` | Head number |
| `DL` | `0x80` | Drive: `0x80`–`0x87` = hard disks, `0x00` = floppy |
| `ES:BX` | `0x0000:0x1000` | Destination in memory = linear address 0x1000 |

> **Why `dl = 0x80`?** IBM's BIOS convention: floppy drives are `0x00`, `0x01`… Hard drives are `0x80`, `0x81`… The first hard disk is always `0x80`.

> **Why `cl = 2`?** Disk sectors are numbered starting at 1. Sector 1 is the 512-byte boot sector (our bootloader). The kernel starts at sector 2.

After the call, the BIOS sets the **carry flag** (CF) if there was an error. `jc disk_error` means "jump if carry flag is set."

### Lines 39–41: Continue after successful load

```nasm
    mov si, loaded_msg
    call print_string
    jmp enable_a20
```
Print a success message then continue. `si` is the **source index** register — `print_string` uses it to read characters one by one.

### Lines 44–53: The A20 line

```nasm
enable_a20:
    mov ax, 0x2401
    int 0x15
    jc a20_error
    jmp load_gdt
```

**What is the A20 line?** Very old PCs (IBM PC/AT, 1982) had a bug where accessing memory above 1 MB would wrap around back to address 0. Later machines added a gate (called the A20 gate, controlling address bit 20) that had to be explicitly opened to access RAM above 1 MB. Even modern PCs start with this gate closed for backwards compatibility.

`int 0x15, AX=0x2401` — BIOS function to enable (open) the A20 gate. If carry is set afterward, it failed.

### Lines 56–58: Loading the GDT

```nasm
load_gdt:
    lgdt [gdt_descriptor]
    jmp switch_to_protected
```
`lgdt` loads the GDT pointer register with the 6-byte structure we defined in `gdt.asm`. After this, the GDT is ready for the CPU to use.

### Lines 61–68: Entering protected mode

```nasm
switch_to_protected:
    cli
    mov eax, cr0
    or eax, 1          ; set bit 0 (Protection Enable)
    mov cr0, eax
    jmp 0x08:protected_mode
```

**CR0** is a **control register** — a special CPU register that controls operating modes.  
Bit 0 of CR0 is the **PE (Protection Enable)** bit. When you set it to 1 the CPU switches to protected mode on the very next instruction.

`or eax, 1` sets only bit 0 and leaves all other bits unchanged. In binary, the number `1` is `0000...0001`, so OR-ing with it flips only bit 0 to 1.

`jmp 0x08:protected_mode` — a **far jump**. It loads the code segment register (`CS`) with `0x08` (our kernel code segment selector from the GDT) and jumps to the label `protected_mode`. This is required because the CPU's internal instruction pipeline may have already fetched the next instruction in 16-bit mode; the far jump flushes that pipeline and forces it to re-decode in 32-bit mode.

### Lines 70–84: Running in protected mode

```nasm
[BITS 32]
protected_mode:
    mov ax, 0x10            ; data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9C000        ; set new stack

    call 0x1000             ; jump to kernel!
    cli
    hlt
```

- `[BITS 32]` — from here on NASM generates 32-bit instructions.
- `0x10` is the data segment selector (GDT index 2). All data segments are set to this so every data access uses the full 4 GB flat segment.
- `esp = 0x9C000` — a safe region for the stack, well below the start of BIOS memory at 0xA0000 and well above the kernel at 0x1000.
- `call 0x1000` — jump to the kernel! The kernel was loaded at 0x1000 and its first instruction is `_start` in `entry.asm`.

### Lines 87–97: Print helper and halt

```nasm
print_string:
    lodsb                   ; load byte from [si] into al, increment si
    test al, al             ; check if al == 0 (end of string)
    jz .done
    mov ah, 0x0E            ; BIOS teletype print function
    int 0x10                ; BIOS video interrupt
    jmp print_string
.done:
    ret
```

- `lodsb` — loads one byte from the address in `si` into `AL`, then adds 1 to `si`. On the next iteration it reads the next byte.
- `test al, al` — sets the zero flag if `al` is 0. This checks for the null terminator at the end of the string.
- `int 0x10, AH=0x0E` — BIOS "teletype" print: print the character in `AL` at the current cursor position and advance the cursor.

### Lines 106–110: Boot signature

```nasm
times 510 - ($ - $$) db 0   ; pad to 510 bytes
dw 0xAA55                   ; boot signature
```

- `$` = current address, `$$` = start of section. `$ - $$` = how many bytes we have used so far. `510 - ($ - $$)` = how many zero bytes to add to reach byte 510.
- `0xAA55` — **the boot signature**. When the BIOS reads a disk sector, it checks that the last 2 bytes are `0x55` and `0xAA` (stored little-endian, so they appear as `AA 55` in the file). If this signature is missing the BIOS refuses to boot. This is how the BIOS distinguishes a bootable disk from a random data disk.

The total is exactly **512 bytes**: 510 bytes of code/data + 2 bytes of signature.

---

## 6. Kernel Entry — `boot/entry.asm`

```nasm
[bits 32]
[extern kmain]
[global _start]

_start:
    call kmain
.hang:
    cli
    hlt
    jmp .hang
```

This is the **bridge between the bootloader and the C kernel**. It is written in assembly because the linker needs an assembly-controlled `_start` symbol to guarantee it lands exactly at the top of the kernel binary (address 0x1000).

- `[bits 32]` — we are already in protected mode.
- `[extern kmain]` — `kmain` is defined in a C file (`kmain.c`). `extern` tells NASM "trust me, the linker will find this symbol."
- `[global _start]` — make `_start` visible to the linker so `-e _start` in the Makefile can find it.
- `call kmain` — call the C function `kmain`. The C compiler handles the calling convention (how arguments and return values work).
- `.hang` — if `kmain` ever returns (it shouldn't), we disable interrupts and halt. The `jmp .hang` catches the (impossible) case where `hlt` returns, looping forever.

---

## 7. ISR Stubs — `boot/isr_stubs.s`

### Background: What is an interrupt?

An **interrupt** is a signal that tells the CPU "stop what you are doing, handle this event, then come back." There are two sources:

1. **CPU exceptions** (interrupts 0–31) — errors the CPU itself detects, like dividing by zero, accessing invalid memory, etc.
2. **Hardware IRQs** (interrupt requests) — signals from devices. The keyboard sends an IRQ when a key is pressed. The timer sends an IRQ 18 times per second.

When an interrupt fires, the CPU:
1. Pushes `SS`, `ESP`, `EFLAGS`, `CS`, `EIP` onto the stack (the return address and CPU state).
2. Looks up the handler address in the **IDT** (Interrupt Descriptor Table).
3. Jumps to that handler.
4. When the handler executes `iret`, the CPU pops those values back and resumes where it left off.

The problem: our C handler functions (`isr_handler`, `irq_handler`) need all the CPU register values saved so they can examine what was happening when the interrupt fired. The CPU only saves a few registers automatically; the stub saves the rest.

### The macros

```asm
.macro ISR_NOERRCODE num
.global isr\num
isr\num:
    pushl $0        ; push dummy error code (CPU did not push one)
    pushl $\num     ; push the interrupt number
    jmp isr_common_stub
.endm
```

For interrupts where the CPU does **not** push an error code automatically, we push `$0` as a placeholder. This keeps the stack frame the same shape for all interrupts so the C handler struct works.

```asm
.macro ISR_ERRCODE num
.global isr\num
isr\num:
    pushl $\num     ; CPU already pushed error code; just push interrupt number
    jmp isr_common_stub
.endm
```

For interrupts where the CPU **does** push an error code (8, 10, 11, 12, 13, 14, 17, 30), we skip the dummy push.

**Which CPU exceptions push an error code?**

| INT# | Name | Error code? |
|------|------|-------------|
| 0 | Divide-by-zero | No |
| 6 | Invalid opcode | No |
| 8 | Double fault | Yes |
| 10 | Invalid TSS | Yes |
| 11 | Segment not present | Yes |
| 12 | Stack fault | Yes |
| 13 | General protection fault | Yes |
| 14 | Page fault | Yes |
| 17 | Alignment check | Yes |
| 30 | Security exception | Yes |

```asm
.macro IRQ num mapped
.global irq\num
irq\num:
    pushl $0        ; dummy error code
    pushl $\mapped  ; the remapped interrupt number (32–47)
    jmp irq_common_stub
.endm
```

Hardware IRQs use their own stub (`irq_common_stub`) so the handler can tell them apart from CPU exceptions.

### The ISR common stub

```asm
isr_common_stub:
    pusha
```
`pusha` pushes all 8 general-purpose registers: `EAX`, `ECX`, `EDX`, `EBX`, `ESP`, `EBP`, `ESI`, `EDI`. This saves the CPU's working state.

```asm
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
```
Push the four segment registers. These complete the snapshot of the CPU state.

```asm
    movl $0x10, %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
```
Load the kernel data segment (`0x10`) into all data segment registers. The interrupted code might have been using a different segment. We must ensure the C handler runs with the correct kernel segments.

```asm
    pushl %esp          ; push pointer to the saved registers struct
    call isr_handler    ; call the C handler
    addl $4, %esp       ; remove the pointer we just pushed
```
`ESP` now points to the bottom of the saved register block on the stack. Pushing `ESP` passes a pointer to that block as the first (and only) argument to `isr_handler`. The C function receives this as `struct regs *r`. `addl $4` removes that pointer from the stack after the call returns.

```asm
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa
```
Restore all saved registers in reverse order.

```asm
    addl $8, %esp   ; discard int_no and err_code
    iret
```
The interrupt number and error code (8 bytes total, 2 × 4-byte integers) were pushed by the stub before `isr_common_stub` was called. We skip over them. Then `iret` (**interrupt return**) pops `EIP`, `CS`, and `EFLAGS` and resumes the interrupted code exactly where it left off.

The stack layout just before `iret` looks like this (top = lowest address):

```
[ESP + 0 ] EIP      ← pushed by CPU when interrupt fired
[ESP + 4 ] CS       ← pushed by CPU
[ESP + 8 ] EFLAGS   ← pushed by CPU
```

`iret` consumes all three and resumes execution.

---

## 8. IDT Header — `kernel/src/utils/idt.h`

### The IDT entry struct

```c
struct idt_entry {
    uint16_t base_low;   // lower 16 bits of handler function address
    uint16_t sel;        // code segment selector (always 0x08 for kernel)
    uint8_t  always0;   // reserved, must be 0
    uint8_t  flags;      // type and attribute byte
    uint16_t base_high;  // upper 16 bits of handler function address
} __attribute__((packed));
```

Each entry is **8 bytes**. It describes one interrupt handler. The handler's address is split into `base_low` (bits 0–15) and `base_high` (bits 16–31).

`__attribute__((packed))` tells GCC: do not add any padding bytes between fields. Without it GCC might add padding for alignment, which would break the exact 8-byte structure the CPU expects.

### The IDT pointer struct

```c
struct idt_ptr {
    uint16_t limit;   // size of the IDT in bytes, minus 1
    uint32_t base;    // address of the IDT array in memory
} __attribute__((packed));
```

This 6-byte structure is loaded into the CPU's IDTR register with the `lidt` instruction, the same way the GDTR is loaded with `lgdt`.

---

## 9. IDT Implementation — `kernel/src/utils/idt.c`

### The IDT array

```c
struct idt_entry idt[256];
struct idt_ptr idtp;
```

256 entries because x86 supports interrupt vectors 0 through 255.

### idt_set_gate

```c
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel       = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}
```

- `base & 0xFFFF` — keep only the lower 16 bits of the address. `0xFFFF` in binary is 16 ones; AND-ing with it zeroes out everything above bit 15.
- `(base >> 16) & 0xFFFF` — shift right by 16, bringing the upper 16 bits down, then mask to keep only those 16 bits.
- `sel = 0x08` — the kernel code segment selector (always).
- `flags = 0x8E` (decoded below).

#### Flag byte `0x8E` decoded

`0x8E` in binary is `1000 1110`.

| Bits | Name | Value | Meaning |
|------|------|-------|---------|
| 7 | P (Present) | 1 | This gate is active |
| 6–5 | DPL | 00 | Ring 0 only — user code cannot trigger these |
| 4 | 0 | 0 | Must be 0 for interrupt gates |
| 3–0 | Type | 1110 | `0xE` = 32-bit interrupt gate |

A **32-bit interrupt gate** automatically disables further interrupts when it runs (clears the interrupt flag). This prevents an interrupt handler from itself being interrupted before it finishes.

### PIC remapping — `pic_remap()`

**What is the PIC?** The **Programmable Interrupt Controller** (Intel 8259A) is a chip that manages hardware IRQ signals and routes them to the CPU as interrupt vectors.

**The problem:** The CPU has 32 reserved exception vectors (0–31). The original IBM PC mapped hardware IRQs 0–15 directly on top of vectors 8–15, which **collides** with CPU exceptions like "double fault" (8) and "general protection fault" (13). When a keyboard interrupt arrives, the CPU would think it was a CPU exception.

**The fix:** reprogram the PIC to map IRQs to vectors 32–47 (beyond the CPU exception range).

```c
outb(0x20, 0x11);   // send ICW1 to master PIC command port
outb(0xA0, 0x11);   // send ICW1 to slave  PIC command port
```

**PIC port addresses:**

| Port | Name | Purpose |
|------|------|---------|
| `0x20` | Master PIC command | Send commands to master PIC |
| `0x21` | Master PIC data | Send data/masks to master PIC |
| `0xA0` | Slave PIC command | Send commands to slave PIC |
| `0xA1` | Slave PIC data | Send data/masks to slave PIC |

**ICW** stands for **Initialization Command Word**. There are 4 of them (ICW1–ICW4) and they must be sent in order.

```c
outb(0x20, 0x11);   // ICW1: 0x11 = initialize + cascade mode + expect ICW4
outb(0xA0, 0x11);   // same for slave
```
`0x11` in binary is `0001 0001`:
- Bit 4 = 1: begin initialization sequence
- Bit 3 = 0: edge-triggered mode
- Bit 1 = 0: cascade mode (master has a slave connected)
- Bit 0 = 1: we will send ICW4

```c
outb(0x21, 0x20);   // ICW2 master: remap to interrupt 0x20 = 32
outb(0xA1, 0x28);   // ICW2 slave:  remap to interrupt 0x28 = 40
```
ICW2 sets the base interrupt vector. After this, IRQ0 → INT 32, IRQ1 → INT 33, …, IRQ7 → INT 39, IRQ8 → INT 40, …, IRQ15 → INT 47.

```c
outb(0x21, 0x04);   // ICW3 master: 0x04 = 0000 0100 = slave on IRQ2
outb(0xA1, 0x02);   // ICW3 slave:  slave's cascade identity = 2
```
IRQ2 of the master PIC is wired to the slave PIC's interrupt line. `0x04` is a bitmask with bit 2 set.

```c
outb(0x21, 0x01);   // ICW4: 0x01 = 8086 mode
outb(0xA1, 0x01);
```
`0x01` enables 8086 mode (as opposed to older 8080 mode). This is always 1 on modern PCs.

```c
outb(0x21, 0xFD);   // mask register: 1111 1101 — only IRQ1 (keyboard) enabled
outb(0xA1, 0xFF);   // mask all slave IRQs
```
After initialization, the **Interrupt Mask Register** (IMR) controls which IRQs are allowed through. A **1** bit **masks** (disables) that IRQ; a **0** bit allows it.

`0xFD` = `1111 1101`:
- Bit 0 = 1 → IRQ0 (timer) masked
- Bit 1 = **0** → IRQ1 (keyboard) **enabled** ← this is the one we want
- Bits 2–7 = 1 → all others masked

`0xFF` = `1111 1111` — all slave IRQs masked. We don't need the real-time clock, serial ports, etc.

### Registering all handlers

```c
idt_set_gate(0,  (uint32_t)isr0,  0x08, 0x8E);
```
For each interrupt vector, we store the address of its stub function. `(uint32_t)isr0` casts the function pointer to a 32-bit integer (the handler's address in memory).

### Loading the IDT

```c
__asm__ volatile("lidt (%0)" : : "r"(&idtp));
```
Inline assembly to execute the `lidt` instruction. `"r"(&idtp)` puts the address of `idtp` into any general-purpose register, and `(%0)` dereferences it (reads the 6 bytes at that address into the IDTR register). After this instruction, the CPU knows where our IDT lives.

---

## 10. ISR/IRQ Handlers — `kernel/src/utils/isr.c`

### Port address constants

```c
#define PIC1_CMD 0x20   // master PIC command port
#define PIC2_CMD 0xA0   // slave  PIC command port
#define PIC_EOI  0x20   // End-of-Interrupt command value
```

`PIC_EOI = 0x20` — writing this value to a PIC command port tells the PIC "I have finished handling that interrupt, you may send the next one." Without sending EOI, the PIC will never fire another interrupt of the same or lower priority.

> Note: `0x20` is used for two unrelated things: the master PIC **port address** and the **EOI command value**. They happen to be the same number but mean different things depending on context.

### The `regs` struct

```c
struct regs {
    unsigned int gs, fs, es, ds;           // segment registers (pushed last in stub)
    unsigned int edi, esi, ebp, esp,       // general purpose (pushed by pusha)
                 ebx, edx, ecx, eax;
    unsigned int int_no, err_code;         // interrupt number and error code (pushed by stub)
    unsigned int eip, cs, eflags,          // pushed automatically by the CPU
                 useresp, ss;
};
```

This struct mirrors **exactly** the stack layout created by the ISR stub. The stub pushed registers in a specific order; this struct reads them in the same order. If the order were different, `r->int_no` would contain the wrong value.

### `isr_handler`

```c
void isr_handler(struct regs *r) {
    (void)r;
    vga_write("CPU exception!\n");
}
```

`(void)r` — suppresses the "unused variable" compiler warning. Currently, the handler just prints a message. A real OS would examine `r->int_no` to print which exception occurred, and potentially kill the offending process.

### `irq_handler`

```c
void irq_handler(struct regs *r) {
    if (r->int_no == 33)
        keyboardHandler();

    if (r->int_no >= 40)
        outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}
```

- `r->int_no == 33` — interrupt 33 is IRQ1, the keyboard interrupt (32 + 1 = 33).
- `r->int_no >= 40` — interrupts 40–47 come from the **slave** PIC (IRQ8–15). For those, we must send EOI to **both** the slave PIC and the master PIC. For interrupts 32–39 (master PIC only), we only send EOI to the master.
- `outb(PIC1_CMD, PIC_EOI)` — always send EOI to master PIC.

---

## 11. I/O Port Helpers — `kernel/src/utils/inb.h`

Modern computers have two ways to talk to hardware devices: **memory-mapped I/O** (the device appears as a range of RAM addresses) and **port I/O** (the device has a separate "port" number accessed with special CPU instructions).

The x86 `in` and `out` instructions perform port I/O. There are no equivalent C operators; you must use inline assembly.

```c
static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
```

- `inb` — **input byte** from port. Reads 1 byte from the hardware port.
- `"inb %1, %0"` — the assembly template. `%0` = first operand (output, `result`), `%1` = second operand (input, `port`).
- `"=a"(result)` — output operand: result is stored in `AL` (the low byte of `EAX`).
- `"Nd"(port)` — input operand: `N` allows small immediate constants (0–255), `d` puts the value in register `DX`. The `inb` instruction takes the port either as an 8-bit immediate or in `DX`.

```c
static inline void outb(uint16_t port, uint8_t data) {
    __asm__ volatile ("outb %0, %1" : : "a"(data), "d"(port));
}
```

- `outb` — **output byte** to port. Writes 1 byte to the hardware port.
- `"a"(data)` — put `data` in `AL`.
- `"d"(port)` — put `port` in `DX`.
- The assembly instruction `outb %AL, %DX` sends the byte in `AL` to the port in `DX`.

`inw`/`outw` are the same but read/write 2 bytes (a **word**) instead of 1.

`static inline` — tells the compiler to insert the function body directly at the call site (no function call overhead). This matters here because port I/O is already slow; adding a function call would double the latency.

---

## 12. VGA Driver Header — `kernel/src/utils/vga.h`

```c
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
```
Standard text mode dimensions. 80 columns × 25 rows = 2000 characters on screen.

### VGA color constants

```c
#define VGA_COLOR_BLACK         0
#define VGA_COLOR_BLUE          1
#define VGA_COLOR_GREEN         2
#define VGA_COLOR_CYAN          3
#define VGA_COLOR_RED           4
#define VGA_COLOR_MAGENTA       5
#define VGA_COLOR_BROWN         6
#define VGA_COLOR_LIGHT_GREY    7
#define VGA_COLOR_DARK_GREY     8
#define VGA_COLOR_LIGHT_BLUE    9
#define VGA_COLOR_LIGHT_GREEN   10
#define VGA_COLOR_LIGHT_CYAN    11
#define VGA_COLOR_LIGHT_RED     12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_LIGHT_BROWN   14
#define VGA_COLOR_WHITE         15
```

VGA text mode supports 16 colors for both foreground (text) and background. They are identified by the numbers 0–15. These are **not** arbitrary; they are the exact values the VGA hardware interprets. Colors 8–15 are the "bright" (high-intensity) versions of colors 0–7.

---

## 13. VGA Driver — `kernel/src/utils/vga.c`

### The VGA buffer

```c
static uint16_t * const VGA_MEMORY = (uint16_t*)0xB8000;
```

`0xB8000` is the **physical memory address** of the VGA text mode buffer. The CPU's address bus is directly wired to the VGA chip at this address. Writing a 16-bit value here immediately updates one character cell on screen — no system call, no driver, no OS required.

The buffer is an array of **2000 16-bit values** (80 × 25). Each 16-bit value represents one character cell:

```
Bits 15–12: background color (4 bits → 16 colors)
Bits  11–8: foreground color (4 bits → 16 colors)
Bits   7–0: ASCII character code
```

Example: `0x0241` = `0000 0010 0100 0001` = background 0 (black), foreground 2 (green), character 0x41 = 'A'.

```c
static int cursor_row = 0;
static int cursor_col = 0;
static uint8_t vga_current_color = 0;
```
Software cursor position (not the hardware cursor blinking on screen, just the position where the next character will be written).

### Color helper

```c
static inline uint8_t vga_make_color(uint8_t fg, uint8_t bg) {
    return (bg << 4) | (fg & 0x0F);
}
```

Packs foreground and background color numbers into a single byte:
- `bg << 4` — shift background color left by 4 bits (into the upper nibble)
- `fg & 0x0F` — mask foreground to 4 bits (lower nibble)
- OR them together

Example: `vga_make_color(2, 0)` = `(0 << 4) | (2 & 0x0F)` = `0x02`.

### Character entry helper

```c
static inline uint16_t vga_entry(char c, uint8_t color) {
    return ((uint16_t)color << 8) | (uint8_t)c;
}
```

Packs a character and its color byte into a 16-bit VGA cell:
- `color << 8` — color byte in the high 8 bits
- `(uint8_t)c` — ASCII value in the low 8 bits

Example: `vga_entry('A', 0x02)` = `0x0241`.

### vga_init

```c
void vga_init() {
    vga_set_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
    vga_clear();
}
```
Set the color to green text on black background, then clear the screen.

### vga_clear

```c
void vga_clear() {
    for (int row = 0; row < VGA_HEIGHT; row++)
        for (int col = 0; col < VGA_WIDTH; col++)
            VGA_MEMORY[row * VGA_WIDTH + col] = vga_entry(' ', vga_current_color);
    cursor_row = 0;
    cursor_col = 0;
}
```

Fills every cell with a space character using the current color. `row * VGA_WIDTH + col` converts a (row, col) pair into a flat array index — standard 2D-to-1D index formula.

### vga_scroll

```c
static void vga_scroll() {
    for (int row = 1; row < VGA_HEIGHT; row++)
        for (int col = 0; col < VGA_WIDTH; col++)
            VGA_MEMORY[(row - 1) * VGA_WIDTH + col] = VGA_MEMORY[row * VGA_WIDTH + col];
    // clear last row
    for (int col = 0; col < VGA_WIDTH; col++)
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + col] = vga_entry(' ', vga_current_color);
    cursor_row = VGA_HEIGHT - 1;
    cursor_col = 0;
}
```

When the cursor reaches the bottom row, the screen scrolls:
1. Copy each row upward by one position (row 1 → row 0, row 2 → row 1, …).
2. Fill the last row with spaces.
3. Position the cursor at the start of the (now empty) last row.

### vga_putchar

```c
void vga_putchar(char c) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
    } else if (c == '\b') {
        if (cursor_col > 0) cursor_col--;
        VGA_MEMORY[cursor_row * VGA_WIDTH + cursor_col] = vga_entry(' ', vga_current_color);
    } else {
        VGA_MEMORY[cursor_row * VGA_WIDTH + cursor_col] = vga_entry(c, vga_current_color);
        cursor_col++;
        if (cursor_col >= VGA_WIDTH) {
            cursor_col = 0;
            cursor_row++;
        }
    }
    if (cursor_row >= VGA_HEIGHT)
        vga_scroll();
}
```

- `'\n'` (newline, ASCII 10) — move cursor to the start of the next row.
- `'\b'` (backspace, ASCII 8) — move cursor left by one, then overwrite that cell with a space (erase the character).
- Normal character — write the character at the current cursor position, advance column. If column reaches 80, wrap to the next row.
- After any change, if `cursor_row >= 25`, scroll.

---

## 14. Keyboard Driver Header — `kernel/src/utils/keyboard.h`

### What is a scancode?

When you press a key, the keyboard controller sends a **scancode** — a number identifying that physical key — to the CPU over I/O port `0x60`. The scancode is independent of any language or character mapping. The OS is responsible for translating scancodes to characters.

When you **release** a key, a second scancode is sent: the press scancode with bit 7 set (i.e., with `0x80` added). For example, pressing 'A' sends `0x1E`; releasing 'A' sends `0x9E` (`0x1E | 0x80`).

### Scancode table (complete)

All values below are **Set 1** (IBM PC AT) scancodes — the default set used by PS/2 keyboards:

| Hex | Key | Hex | Key | Hex | Key |
|-----|-----|-----|-----|-----|-----|
| `0x01` | ESC | `0x02` | 1 | `0x03` | 2 |
| `0x04` | 3 | `0x05` | 4 | `0x06` | 5 |
| `0x07` | 6 | `0x08` | 7 | `0x09` | 8 |
| `0x0A` | 9 | `0x0B` | 0 | `0x0C` | - _ |
| `0x0D` | = + | `0x0E` | Backspace | `0x0F` | Tab |
| `0x10` | Q | `0x11` | W | `0x12` | E |
| `0x13` | R | `0x14` | T | `0x15` | Y |
| `0x16` | U | `0x17` | I | `0x18` | O |
| `0x19` | P | `0x1A` | [ { | `0x1B` | ] } |
| `0x1C` | Enter | `0x1D` | Left Ctrl | `0x1E` | A |
| `0x1F` | S | `0x20` | D | `0x21` | F |
| `0x22` | G | `0x23` | H | `0x24` | J |
| `0x25` | K | `0x26` | L | `0x27` | ; : |
| `0x28` | ' " | `0x29` | ` ~ | `0x2A` | Left Shift |
| `0x2C` | Z | `0x2D` | X | `0x2E` | C |
| `0x2F` | V | `0x30` | B | `0x31` | N |
| `0x32` | M | `0x33` | , < | `0x34` | . > |
| `0x35` | / ? | `0x36` | Right Shift | `0x38` | Left Alt |
| `0x39` | Space | `0x3A` | Caps Lock | `0x3B` | F1 |
| `0x3C` | F2 | `0x3D` | F3 | `0x3E` | F4 |
| `0x3F` | F5 | `0x40` | F6 | `0x41` | F7 |
| `0x42` | F8 | `0x43` | F9 | `0x44` | F10 |
| `0x47` | Home | `0x48` | Up Arrow | `0x49` | Page Up |
| `0x4B` | Left Arrow | `0x4D` | Right Arrow | `0x4F` | End |
| `0x50` | Down Arrow | `0x51` | Page Down | `0x52` | Insert |
| `0x53` | Delete | `0x57` | F11 | `0x58` | F12 |

Extended keys (Right Alt `0xE038`, Right Ctrl `0xE01D`) are prefixed with the byte `0xE0` before the scancode, which requires special multi-byte handling not yet implemented.

---

## 15. Keyboard Driver — `keyboard/src/utils/keyboard.c`

### Lookup table

```c
char ascii_normal[256] = {0};
```

An array of 256 characters, indexed by scancode. Initially all zeros (no mapping). `keyboard_init` fills in the entries for recognized scancodes.

```c
ascii_normal[scanA] = 'a';   // scanA = 0x1E, so ascii_normal[0x1E] = 'a'
```

After initialization, you can look up any scancode like this:
```c
char c = ascii_normal[scancode];   // if scancode is 0x1E, c = 'a'
```

### Circular buffer

```c
static char keyboard_buffer[256];
static int buffer_head = 0;
static int buffer_tail = 0;
```

A **circular buffer** (also called a ring buffer) stores characters that have been typed but not yet read by `kmain`. It has two pointers:
- `buffer_head` — where the next incoming character will be **written**
- `buffer_tail` — where the next character will be **read** from

```
[ . ][ . ][ . ][ a ][ b ][ c ][ . ][ . ]
                  ^               ^
               tail (read here) head (write here)
```

When `buffer_head == buffer_tail`, the buffer is **empty**. The buffer wraps around using modulo 256: `(head + 1) % 256`. If the buffer fills completely (head wraps around to tail), older keystrokes are silently overwritten.

### keyboardHandler

```c
void keyboardHandler(void) {
    uint8_t scancode = inb(0x60);
    if (scancode & 0x80)     // key-release events have bit 7 set
        return;
    char c = ascii_normal[scancode];
    keyboard_buffer[buffer_head] = c;
    buffer_head = (buffer_head + 1) % 256;
}
```

- `inb(0x60)` — read the scancode from the keyboard data port.
- `scancode & 0x80` — check bit 7. If it is 1, this is a **key-release** event. `0x80` in binary is `1000 0000`; AND-ing with it isolates bit 7.
- If bit 7 is 0 (key press), look up the ASCII character and store it in the buffer.
- `(buffer_head + 1) % 256` — advance head, wrapping at 256.

### keyboard_getchar

```c
char keyboard_getchar(void) {
    if (buffer_head == buffer_tail)
        return 0;                   // buffer empty, nothing to return
    char c = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % 256;
    return c;
}
```

Reads and removes the oldest character from the buffer. Returns `0` (null character) if the buffer is empty, which `kmain` checks for.

---

## 16. Kernel Main — `kernel/src/manager/kmain.c`

```c
#include "../utils/vga.h"
#include "../utils/keyboard.h"
#include "../utils/idt.h"
```

Include the headers that declare the functions we will call. The paths are relative to the file's location.

```c
void kmain(void) {
```
The C entry point. Called from `entry.asm`. Takes no arguments (`void`).

```c
    vga_init();
```
Clear the screen and set green-on-black colors. This is the first thing we do so we have somewhere to print output.

```c
    keyboard_init();
```
Fill the `ascii_normal` lookup table. Must be called before any keyboard input is read.

```c
    idt_install();
```
Set up all 256 IDT entries, remap the PIC, and load the IDT register. After this returns, the CPU knows how to handle interrupts.

```c
    vga_write("Loading kernel\n");
    vga_write("Kernel is running successfully!\n");
    vga_write("To test the keyboard type characters\n");
    vga_write("\n");
```
Print startup messages to the VGA screen. These are the first characters the user sees.

```c
    __asm__ volatile("sti");
```
**Set Interrupt flag** — enable hardware interrupts. We deliberately waited until after `idt_install()` to do this. If we had enabled interrupts before setting up the IDT, a keyboard press or timer tick could trigger an interrupt with no handler, causing a CPU fault.

```c
    for (;;) {
        char c = keyboard_getchar();
        if (c != 0) {
            vga_putchar(c);
        }
    }
```
**The main loop** — runs forever. On each iteration:
1. Call `keyboard_getchar()`. If the buffer is empty it returns `0`.
2. If a character was available (`c != 0`), print it to the screen.

The keyboard interrupt handler (`keyboardHandler`) fills the buffer asynchronously — it runs whenever a key is pressed, regardless of where in the main loop we are. The main loop drains that buffer and displays each character.

---

## 17. Full Boot-to-Keypress Flow

```
Power on
   │
   ▼
BIOS wakes up, reads sector 0 of the disk (512 bytes) into RAM at 0x7C00
   │
   ▼
boot.asm runs in 16-bit real mode:
  1. Sets up segment registers and stack
  2. Calls BIOS INT 0x13 → loads 8 sectors (kernel) into RAM at 0x1000
  3. Calls BIOS INT 0x15 → enables A20 gate
  4. lgdt → loads GDT from gdt.asm data
  5. Sets CR0 bit 0 → switches to 32-bit protected mode
  6. Far jump to code segment 0x08 → flushes instruction pipeline
   │
   ▼
boot.asm in 32-bit protected mode:
  7. Sets ds/es/fs/gs/ss = 0x10 (kernel data segment)
  8. Sets esp = 0x9C000 (stack)
  9. call 0x1000 → jumps to kernel
   │
   ▼
entry.asm (_start at 0x1000):
  10. call kmain
   │
   ▼
kmain.c:
  11. vga_init() → clears screen, sets green/black
  12. keyboard_init() → fills scancode→ASCII table
  13. idt_install():
        - Fills all 256 IDT entries with stub addresses
        - pic_remap(): reprograms PIC → IRQ0-7=INT32-39, IRQ8-15=INT40-47
        - lidt → loads IDT register
  14. Prints startup messages
  15. sti → enables interrupts
  16. Enters infinite loop calling keyboard_getchar()
   │
   ┌──────────────────────────────────────────┐
   │         (when a key is pressed)          │
   │                                          │
   │  Keyboard → IRQ1 → INT 33               │
   │  CPU: saves EIP/CS/EFLAGS on stack       │
   │  CPU: looks up INT 33 in IDT             │
   │  CPU: jumps to irq1 stub                 │
   │                                          │
   │  irq_common_stub:                        │
   │    pusha, push segments                  │
   │    load kernel data segment              │
   │    call irq_handler(struct regs*)        │
   │                                          │
   │  irq_handler:                            │
   │    if int_no == 33: keyboardHandler()    │
   │      → inb(0x60) reads scancode          │
   │      → looks up ascii_normal[scancode]   │
   │      → stores char in keyboard_buffer    │
   │    outb(PIC1_CMD, PIC_EOI)               │
   │                                          │
   │  irq_common_stub returns:                │
   │    pop segments, popa                    │
   │    iret → resumes main loop              │
   │                                          │
   └──────────────────────────────────────────┘
   │
   ▼
  Main loop sees character in buffer
  keyboard_getchar() returns it
  vga_putchar() writes it to 0xB8000
  Character appears on screen
```

---

## 18. Quick-Reference: Every Hex Value in the Codebase

### Memory Addresses

| Hex | Meaning |
|-----|---------|
| `0x7C00` | BIOS loads the boot sector here |
| `0x1000` | Kernel is loaded here by bootloader |
| `0x9C000` | Kernel stack top (grows downward) |
| `0xB8000` | VGA text mode buffer |

### BIOS Interrupt Calls

| INT | AH/AX | Purpose |
|-----|-------|---------|
| `0x10` | `0x0E` | Teletype print character (AL = char) |
| `0x13` | `0x02` | Read disk sectors into memory |
| `0x15` | `0x2401` | Enable A20 line |

### Disk Read Registers (INT 0x13, AH=0x02)

| Register | Value | Meaning |
|----------|-------|---------|
| `DL` | `0x80` | First hard disk |
| `DL` | `0x00` | First floppy disk |
| `CL` | `0x02` | Start reading from sector 2 |
| `BX` | `0x1000` | Load into address 0x1000 |

### GDT Access Bytes

| Byte | Binary | Segment Type |
|------|--------|--------------|
| `0x9A` | `1001 1010` | Kernel code (ring 0, executable, readable) |
| `0x92` | `1001 0010` | Kernel data (ring 0, writable) |

### GDT Granularity Byte

| Byte | Binary | Meaning |
|------|--------|---------|
| `0xCF` | `1100 1111` | 32-bit, 4KB granularity, 4GB segment |

### Segment Selectors

| Value | Segment |
|-------|---------|
| `0x00` | Null descriptor (never use) |
| `0x08` | Kernel code segment (GDT index 1) |
| `0x10` | Kernel data segment (GDT index 2) |

### CPU Control

| Value/Instruction | Meaning |
|-------------------|---------|
| `CR0 \| 1` | Set PE bit → enter protected mode |
| `cli` | Disable interrupts |
| `sti` | Enable interrupts |
| `hlt` | Halt CPU until next interrupt |
| `iret` | Return from interrupt (restores EIP/CS/EFLAGS) |

### IDT Gate Flags

| Byte | Binary | Meaning |
|------|--------|---------|
| `0x8E` | `1000 1110` | Present, ring 0, 32-bit interrupt gate |

### PIC Port Addresses

| Port | Name |
|------|------|
| `0x20` | Master PIC command register |
| `0x21` | Master PIC data / IMR register |
| `0xA0` | Slave PIC command register |
| `0xA1` | Slave PIC data / IMR register |

### PIC ICW Values

| Value | Meaning |
|-------|---------|
| `0x11` | ICW1: initialize, cascade, expect ICW4 |
| `0x20` | ICW2 master: map IRQ0 → INT 32 |
| `0x28` | ICW2 slave: map IRQ8 → INT 40 |
| `0x04` | ICW3 master: slave on IRQ2 line |
| `0x02` | ICW3 slave: cascade identity = 2 |
| `0x01` | ICW4: 8086 mode |
| `0xFD` | IMR master: `1111 1101` — only IRQ1 (keyboard) unmasked |
| `0xFF` | IMR slave: all IRQs masked |
| `0x20` | EOI (End of Interrupt) command |

### PIC Interrupt Mapping (after remapping)

| IRQ | INT vector | Device |
|-----|-----------|--------|
| IRQ0 | 32 (0x20) | Programmable Interval Timer |
| IRQ1 | 33 (0x21) | PS/2 Keyboard |
| IRQ2 | 34 | Cascade (slave PIC) |
| IRQ3 | 35 | COM2 serial port |
| IRQ4 | 36 | COM1 serial port |
| IRQ5 | 37 | LPT2 / Sound card |
| IRQ6 | 38 | Floppy disk |
| IRQ7 | 39 | LPT1 / Spurious |
| IRQ8 | 40 | Real-time clock |
| IRQ12 | 44 | PS/2 Mouse |
| IRQ14 | 46 | Primary ATA hard disk |
| IRQ15 | 47 | Secondary ATA hard disk |

### Keyboard Port

| Port | Direction | Meaning |
|------|-----------|---------|
| `0x60` | Read | Keyboard scancode |
| `0x60` | Write | Command to keyboard controller |
| `0x64` | Read | Keyboard controller status |
| `0x64` | Write | Command to keyboard controller |

### Keyboard Scancode Bit

| Bit mask | Value | Meaning |
|----------|-------|---------|
| `0x80` | bit 7 set | Key **release** event (ignore) |
| `0x80` | bit 7 clear | Key **press** event (handle) |

### VGA Colors

| Value | Color |
|-------|-------|
| 0 | Black |
| 1 | Blue |
| 2 | Green |
| 3 | Cyan |
| 4 | Red |
| 5 | Magenta |
| 6 | Brown |
| 7 | Light Grey |
| 8 | Dark Grey |
| 9 | Light Blue |
| 10 | Light Green |
| 11 | Light Cyan |
| 12 | Light Red |
| 13 | Light Magenta |
| 14 | Yellow (Light Brown) |
| 15 | White |

### VGA Cell Layout (16-bit value)

```
Bit: 15  14  13  12  11  10   9   8   7   6   5   4   3   2   1   0
     [    background    ] [    foreground    ] [       ASCII code      ]
     [    color 0-15    ] [    color 0-15    ] [      character        ]
```

Example: `0x0241` = black background, green text, 'A'

### Boot Sector

| Value | Meaning |
|-------|---------|
| `0xAA55` | Boot signature (must be at bytes 510–511 of sector) |
| `510` | Total usable bytes before signature |
| `512` | Total sector size in bytes |



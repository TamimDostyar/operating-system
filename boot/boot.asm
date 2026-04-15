; BuddyOS Bootloader
; Loads kernel from disk, switches to 32-bit protected mode, jumps to kernel.

[bits 16]
[ORG 0x7C00]

jmp start

%include "boot/gdt.asm"

start:
    cli                     ; Disable interrupts while we set up
    xor ax, ax              ; ax is the location in the CPU same for ds, es, ss, sp
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00          ; Stack grows down from 0x7C00
    sti                     ; Re-enable interrupts

    mov si, boot_msg
    call print_string


load_kernel:
    mov ah, 0x02            ; BIOS function: read sectors
    mov al, 4                ; Number of sectors to read (2KB — enough for current kernel)
    mov ch, 0               ; Cylinder 0
    mov cl, 2               ; Sector 2 (sector 1 is bootloader itself)
    mov dh, 0               ; Head 0
    mov dl, 0x80            ; Drive 0x80 = first hard disk (change to 0x00 for floppy)
    xor bx, bx
    mov es, bx              ; Destination segment = 0x0000
    mov bx, 0x1000          ; Destination offset = 0x1000 (so address = 0x0000:0x1000 = 0x1000 linear)
    int 0x13
    jc disk_error           ; If carry set, error occurred

    ; Success: print "Kernel loaded" message
    mov si, loaded_msg
    call print_string
    jmp enable_a20          ; Continue to protected mode setup


disk_error:
    mov si, disk_err_msg
    call print_string
    jmp halt


enable_a20:
    mov ax, 0x2401
    int 0x15
    jc a20_error
    jmp load_gdt

a20_error:
    mov si, a20_err_msg
    call print_string
    jmp halt

; Load Global Descriptor Table
load_gdt:
    lgdt [gdt_descriptor]
    jmp switch_to_protected


; Switch to 32-bit protected mode
switch_to_protected:
    cli
    mov eax, cr0
    or eax, 1               ; Set PE (Protection Enable) bit
    mov cr0, eax
    jmp 0x08:protected_mode ; Far jump to 32-bit code segment

[BITS 32]
protected_mode:
    ; Set up data segments
    mov ax, 0x10            ; Data segment selector (index 2 in GDT)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9C000        ; Set stack pointer (safe area below 1MB)

    ; Call kernel entry point (kmain)
    ; The kernel binary was loaded at 0x1000
    call 0x1000

    ; If kernel returns, hang
    cli
    hlt


; helper function
[BITS 16]
print_string:
    lodsb                   ; Load byte from [si] into al, increment si
    test al, al
    jz .done
    mov ah, 0x0E            ; BIOS teletype output
    int 0x10
    jmp print_string
.done:
    ret

halt:
    cli
    hlt
    jmp halt

; the messages we would need to show for each cases

boot_msg       db "BuddyOS bootloader starting...", 0x0D, 0x0A, 0
loaded_msg     db "Kernel loaded. Switching to protected mode...", 0x0D, 0x0A, 0
disk_err_msg   db "ERROR: Disk read failed!", 0x0D, 0x0A, 0
a20_err_msg    db "ERROR: A20 line enable failed!", 0x0D, 0x0A, 0


times 510 - ($ - $$) db 0   ; Pad to 510 bytes
dw 0xAA55                   ; Boot signature
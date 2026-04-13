; BuddyOS — minimal BIOS boot sector.
; Assembled as a raw 512-byte binary and loaded by BIOS at 0x7C00.

[BITS 16]
[ORG 0x7C00]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov si, boot_msg

print_loop:
    lodsb
    test al, al
    jz halt
    mov ah, 0x0E
    int 0x10
    jmp print_loop

halt:
    cli
    hlt
    jmp halt

boot_msg db "BuddyOS booted - under construction though!", 0

times 510 - ($ - $$) db 0
dw 0xAA55

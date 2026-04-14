; Global Descriptor Table (GDT)
; Included by the bootloader before switching to protected mode.

[bits 16]

; defines memory segments for protected mode (8 bytes into binary all zeros)
gdt_start:
    dq 0x0000000000000000

gdt_code:   ; Code segment descriptor
    dw 0xFFFF               ; Limit low 16 bits
    dw 0x0000               ; Base low 16 bits
    db 0x00                 ; Base middle 8 bits
    db 0x9A                 ; Access: present, ring0, code, executable, readable
    db 0xCF                 ; Granularity: 4KB pages, 32-bit, limit high 4 bits = 0xF
    db 0x00                 ; Base high 8 bits

gdt_data:   ; Data segment descriptor
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92                 ; Access: present, ring0, data, writable
    db 0xCF
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1   ; Size of GDT minus 1
    dd gdt_start                  ; Linear address of GDT


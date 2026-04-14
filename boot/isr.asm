extern isr_handler

%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push byte 0    ; Push dummy error code
    push byte %1   ; Push interrupt number
    jmp isr_common
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push byte %1
    jmp isr_common
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1

isr_common:
    pusha          ; Push all registers
    call isr_handler
    popa
    add esp, 8 
    sti
    iret
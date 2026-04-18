.code32

.extern isr_handler
.extern irq_handler

/* CPU exceptions that do NOT push an error code: push a dummy 0 first */
.macro ISR_NOERRCODE num
.global isr\num
isr\num:
    pushl $0
    pushl $\num
    jmp isr_common_stub
.endm

/* CPU exceptions that DO push an error code: CPU already pushed it */
.macro ISR_ERRCODE num
.global isr\num
isr\num:
    pushl $\num
    jmp isr_common_stub
.endm

/* IRQ stubs: push dummy error code + remapped interrupt number */
.macro IRQ num mapped
.global irq\num
irq\num:
    pushl $0
    pushl $\mapped
    jmp irq_common_stub
.endm

/* CPU exceptions 0-31 */
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE   30
ISR_NOERRCODE 31

/* IRQs 0-15 remapped to interrupts 32-47 */
IRQ  0  32
IRQ  1  33
IRQ  2  34
IRQ  3  35
IRQ  4  36
IRQ  5  37
IRQ  6  38
IRQ  7  39
IRQ  8  40
IRQ  9  41
IRQ 10  42
IRQ 11  43
IRQ 12  44
IRQ 13  45
IRQ 14  46
IRQ 15  47

isr_common_stub:
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    movl $0x10, %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    pushl %esp
    call isr_handler
    addl $4, %esp
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa
    addl $8, %esp       /* pop int_no and err_code */
    iret

irq_common_stub:
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    movl $0x10, %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    pushl %esp
    call irq_handler
    addl $4, %esp
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa
    addl $8, %esp
    iret

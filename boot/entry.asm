; Kernel entry point — must be linked first so _start lands at 0x1000
[bits 32]
[extern kmain]
[global _start]

_start:
    call kmain
.hang:
    cli
    hlt
    jmp .hang

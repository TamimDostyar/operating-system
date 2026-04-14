To run this, compile first from the root with Nasm:
    - nasm -f bin boot/boot.asm -o boot/boot.bin
    - qemu-system-i386 -drive format=raw,file=boot.bin
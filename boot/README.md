To run this, compile first from the root with Nasm:
    - nasm -f bin boot/boot.asm -o boot/boot.bin
    - qemu-system-i386 -drive format=raw,file=boot.bin

## Goals

- Load the buddyGPT model weights into a reserved memory region at boot, before the kernel takes over
- Eventually boot directly into BuddyShell — no login prompt, no password, just `buddy>`
- Support a boot path that works without any host OS beneath it (bare metal, QEMU for development only)

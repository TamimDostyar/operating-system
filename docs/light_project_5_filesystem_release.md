# Light Project 5: Linux Kernel Module and Custom System Call

## Project Goal
Go inside Linux kernel internals by creating a loadable kernel module and adding a custom system call.

## What I Will Build

- loadable kernel module
- custom /proc entry
- custom system call in kernel source
- user program that invokes the new syscall

## Required Features

1. Kernel module
- Build and load module.
- Expose system info via /proc/tamimos_info (or similar).
- Support clean module unload.

2. Custom system call
- Add syscall implementation in kernel source tree.
- Register syscall number in the correct architecture table.
- Rebuild kernel and boot into the modified kernel.

3. User-space caller
- Write a C program that invokes the new syscall.
- Validate return values and error handling.

4. Documentation
- Keep build and run instructions clear and reproducible.
- Document kernel version and config used.

## Suggested Structure

- kernel-module/tamimos_module.c
- kernel-patch/ (notes and patch files)
- userspace/call_tamimos_syscall.c

## Testing Checklist

- Module inserts and removes without errors.
- /proc entry prints expected info.
- Custom syscall returns expected value.
- Caller program works on modified kernel.

## OSTEP Connection

This project maps to user-kernel boundary, system call flow, and kernel-level programming.

## Deliverable

A modified Linux kernel setup where a custom syscall is running and callable from user space.

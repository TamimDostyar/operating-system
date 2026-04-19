# BuddyOS
<p align="center">
  <img src="asset/cIcon.png" width="32%" />
</p>

![EXAMPLE](asset/kernel.png)

Educational implementations of core operating-system concepts in **C** for **BuddyOS**—an OS built from scratch. The repository is organized as milestones that build the OS in dependency order (boot path, kernel memory, process scheduling, file system, system calls, and shell). A minimal **cross-compiler / emulator** setup (for example QEMU) is only a development convenience, not a dependency on a full desktop OS as the runtime.


## Objectives

- Implement and exercise the same ideas covered in typical OS coursework: execution, heap layout, synchronization, on-disk structure, and the user–kernel boundary—**on BuddyOS**.
- Produce code that can be built, tested, and demonstrated per milestone as BuddyOS grows.
- Keep specifications, checklists, and notes alongside the code in version control.

## Scope

Milestones are defined for **BuddyOS** and intentionally avoid assuming prebuilt kernel subsystems. You implement each layer progressively: bring-up first, then kernel memory, process execution, storage, user-kernel interface, and finally user-facing applications.

## Milestones

| Order | Focus | Summary |
|------:|--------|---------|
| 1 | Bootloader + Kernel | Bootloader load path, protected mode, VGA text, keyboard interrupt |
| 2 | Kernel Allocator | `kmalloc`/`kfree`, free list, coalescing, alignment |
| 3 | Process Scheduler | PCB lifecycle, context switching, round-robin, `fork/exec/wait/exit` |
| 4 | File System | Virtual disk, bitmap, inodes, directories, file operations |
| 5 | System Calls | User-kernel boundary, syscall table, user-mode wrappers and tests |
| 6 | Shell | Built-ins, command execution, redirection, pipeline support |

Detailed requirements and acceptance criteria for each milestone are under `docs/`.

## Repository layout

```
boot/          Bootloader and early boot assembly
kernel/        Core kernel code (bring-up, memory, scheduler, syscalls)
fs/            File-system implementation details (can later merge under kernel/fs)
alloc/         Kernel allocator experiments/notes (can later merge under kernel/mm)
userspace/     User-mode programs and libraries
  shell/       BuddyOS shell app (project 6)
  auth/        Login/authentication app (after syscalls + filesystem)
  edit/        Text editor app (after filesystem + syscalls)
docs/          Sequential project plans and acceptance checklists
OSdocumentation/  Bring-up notes and run instructions
```

Top-level `auth/` and `edit/` folders are currently idea placeholders. As implementation starts, place real sources under `userspace/auth/` and `userspace/edit/` so boundaries stay clear between kernel and apps.

## Prerequisites

Typical expectations per milestone include:

- **Toolchain:** A C toolchain that can produce BuddyOS binaries (GCC/Clang with appropriate flags, `make`, and NASM where assembly is used).
- **Bring-up:** QEMU (or similar) to run the boot image and later kernels, as documented in `OSdocumentation/bootloader-status.md`.
- **Project 4:** BuddyOS block device or RAM-disk path to exercise the on-disk layout.
- **Project 5:** Ability to rebuild the BuddyOS kernel and run user-mode tests that invoke syscalls.
- **Project 6:** User-mode runtime ready so shell/auth/editor apps can call process and file syscalls.

Exact versions and steps are specified in the assignment documents under `docs/`.

## Building and running

There is no single top-level build for every project yet; each project is built and invoked according to its spec. Boot sector build and QEMU steps are in `OSdocumentation/bootloader-status.md`. The shell under `userspace/shell/` may include a local README with run instructions.

## Documentation

Authoritative milestone text, testing checklists, and suggested source layouts live in **`docs/`**.

- Bootloader progress and reproducible run steps: **`OSdocumentation/bootloader-status.md`**.


This project will eventually be used to run BuddyAI, which is currently under development. You can find the BuddyAI repository here: [REPO FOR BUDDY AI](https://github.com/TamimDostyar/buddy/)
The LLM which I will eventually use is trained on my own, the repo can be found on [AI REPOSITORY](https://github.com/TamimDostyar/TD_GPT)


My goal:
  - Create OS from scratch on C
  - Integerate CLI AI entirely in the system
  - Train an entire LLM to perform Google Search and pull the command and run
# TAMIMOS
![CICON](asset/cIcon.png)

Educational implementations of core operating-system concepts in **C** on **Linux**. The repository is organized as a sequence of self-contained milestones (process control, memory management, concurrency, file systems, and kernel interfaces). Each milestone is a runnable artifact on a stock Linux system rather than a single replacement kernel.

Optional use of **C++** is limited to assignments that explicitly require it.

## Objectives

- Implement and exercise the same ideas covered in typical OS coursework: execution, heap layout, synchronization, on-disk structure, and the user–kernel boundary.
- Produce code that can be built, tested, and demonstrated independently per milestone.
- Keep specifications, checklists, and notes alongside the code in version control.

## Scope

All components run **on Linux**. The shell executes real user programs; the allocator can be linked into test binaries; the threading work schedules user-level threads; the file-system milestone is mountable via FUSE; the kernel milestone integrates with the Linux kernel build and module interfaces.

## Milestones

| Order | Focus | Summary |
|------:|--------|---------|
| 1 | Shell | Parser, `fork`/`exec`, built-ins, pipes, I/O redirection |
| 2 | Allocator | `malloc` family semantics, free list, coalescing, alignment |
| 3 | Concurrency | User-level threads, mutexes, condition variables, concurrent data structures |
| 4 | File system | FUSE-backed layout: virtual disk, block bitmap, inodes, directories |
| 5 | Kernel | Loadable module, `procfs`-style exposure, custom system call, user-space test |

Detailed requirements and acceptance criteria for each milestone are under `docs/`.

## Repository layout

```
docs/          Assignment specifications and build notes
src/shell/     Interactive shell (milestone 1)
src/alloc/     Memory allocator (milestone 2; placeholder until implemented)
src/thread/    Threads and synchronization (milestone 3)
src/fs/        File-system implementation (milestone 4)
src/kernel/    Module and syscall-related sources (milestone 5)
```

## Prerequisites

Typical expectations per milestone include:

- **Toolchain:** GCC or Clang, `make` (or equivalent), standard POSIX development headers.
- **Milestone 4:** FUSE development libraries and a kernel that supports FUSE.
- **Milestone 5:** Kernel source matching your running system (or documented version), headers for module builds, and privileges to load modules where permitted.

Exact versions and steps are specified in the assignment documents under `docs/`.

## Building and running

There is no single top-level build yet; each milestone is built and invoked according to its spec. The shell under `src/shell/` may include a local README with run instructions (for example, invoking the `mysh` binary after compilation).

## Documentation

Authoritative milestone text, testing checklists, and suggested source layouts live in **`docs/`**.

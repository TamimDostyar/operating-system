# Light Project 2: Memory Allocator

## Project Goal
Implement a memory allocator in C as a drop-in replacement for the malloc family calls.

## What I Will Build

- my_malloc
- my_free
- my_calloc
- my_realloc

## Required Features

1. Heap source
- Use sbrk() or mmap() to request memory from the OS.

2. Allocation strategy
- Use a free list.
- Choose first-fit or best-fit.

3. Metadata
- Track block size, allocation status, and links.
- Keep alignment correct for returned pointers.

4. Free behavior
- Return blocks to the free list correctly.
- Coalesce adjacent free blocks to reduce fragmentation.

5. Realloc behavior
- Support grow and shrink paths.
- Preserve existing data correctly during resize.

## Suggested Structure

- src/alloc/allocator.c
- src/alloc/allocator.h
- tests/alloc_basic.c
- tests/alloc_stress.c

## Testing Checklist

- Many small allocations and frees.
- Mixed-size allocations.
- Different free orders.
- Realloc grow and shrink.
- calloc zero-initialization checks.

## OSTEP Connection

This project maps directly to heap internals, allocator policies, fragmentation, and memory safety.

## Deliverable

A tested allocator library that can replace malloc family calls in small test programs.

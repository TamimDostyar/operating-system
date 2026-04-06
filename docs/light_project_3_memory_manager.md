# Light Project 3: Thread Library and Synchronization

## Project Goal
Build a user-level thread library, then add synchronization primitives and test with concurrent data structures.

## What I Will Build

- thread_create
- thread_join
- thread_yield
- mutex lock
- condition variable

## Required Features

1. User-level threads
- Create and schedule threads in user space.
- Support cooperative yield.
- Support join semantics for completion.

2. Mutex implementation
- Lock and unlock APIs.
- Protect shared state correctly.

3. Condition variable implementation
- wait and signal/broadcast behavior.
- Correct lock hand-off behavior.

4. Producer-consumer queue
- Bounded queue with multiple producers and consumers.
- No data races.

5. Concurrent hash map
- Shared hash map with synchronized insert and get paths.

## Suggested Structure

- src/thread/thread.c
- src/thread/mutex.c
- src/thread/condvar.c
- src/thread/queue.c
- src/thread/hashmap.c

## Testing Checklist

- Thread creation and join under repeated runs.
- Mutex prevents races in shared counter test.
- Condition variable wakes blocked workers correctly.
- Producer-consumer passes stress tests.
- Hash map stays correct under concurrency.

## OSTEP Connection

This project maps to threading, critical sections, races, locks, and condition variables.

## Deliverable

A working user-level thread library with synchronization primitives and concurrent demo programs.

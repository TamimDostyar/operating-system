
# Process Scheduler

The process scheduler is where BuddyOS tracks processes and chooses which one runs next. Right now it has a process table manager and a simple FIFO ready queue scheduler.

---

## File Overview

```
processScheduler/
└── src/
	├── process.h / process.c      ← process table + process lifecycle
	└── scheduler.h / scheduler.c  ← ready queue + FCFS scheduling
```

---

## `src/process.h` / `src/process.c`

Tracks process metadata (PID, parent PID, state) and manages process lifecycle calls.

| Function | What it does |
|----------|-------------|
| `fork(pid_t, parentPID)` | Creates a process entry in the process table, assigns the first free PID slot (starting from 2), marks it running, and returns the PID |
| `exit_process(manager, pid)` | Marks a PID slot free and resets process state |
| `wait(manager, parentPID)` | Busy-waits until all children of `parentPID` are no longer running |

### Process Structures

| Structure | Purpose |
|----------|---------|
| `Process` | One process record: PID, parent PID, and state |
| `Pid_t` | Process manager: dynamic process table pointer + occupied slot bitmap |

### Process States

| Value | Meaning |
|-------|---------|
| `PROCESS_FREE` | Slot is free |
| `PROCESS_RUNNING` | Process is running |
| `PROCESS_WAITING` | Process is waiting |
| `PROCESS_STOPPED` | Process is stopped |

---

## `src/scheduler.h` / `src/scheduler.c`

Implements a simple first-come-first-served ready queue using a circular buffer.

| Function | What it does |
|----------|-------------|
| `init_scheduler(scheduler)` | Initializes queue head/tail/size and clears current PID |
| `enqueue(scheduler, pid)` | Pushes PID at queue tail (returns `-1` if full) |
| `dequeue(scheduler)` | Pops PID from queue head (returns `-1` if empty) |
| `schedule(scheduler)` | Picks next runnable PID using `dequeue` and sets `currentPID` |

### Scheduler Structure

| Field | Purpose |
|-------|---------|
| `readyQueue[]` | FIFO queue of runnable process IDs |
| `queueHead` | Read index for next process |
| `queueTail` | Write index for new process |
| `queueSize` | Number of items currently in queue |
| `currentPID` | PID currently selected by scheduler |

---

## Current Flow

1. Initialize heap and process manager.
2. `fork()` creates a process and returns a PID.
3. `enqueue()` places PID in ready queue.
4. `schedule()` selects next PID from front of queue.
5. `exit_process()` frees PID slot when process is done.

---

## Build and Run

From the `scheduling` directory:

```bash
You will need to be on the test folder

clang -DTEST -I../scheduling/src -I../alloc/src -o scheduling ../scheduling/src/process.c ../scheduling/src/scheduler.c shedulingTest.c ../alloc/src/kmalloc.c ../alloc/src/heap.c
./main
```

```

---

## Goals

- Integrate scheduler and process manager directly into kernel runtime (not only host-side tests)
- Replace busy-wait `wait()` with timer/interrupt-driven waiting
- Add separate waiting/blocked queues for I/O and synchronization
- Add thread support alongside process scheduling
- Add context switch hooks so `schedule()` can drive real CPU task switching

---

## Architecture

How process tracking and FCFS scheduling are connected today.

```
		 fork(parentPID)
			  │
			  ▼
	  ┌──────────────────┐
	  │  Process Manager │
	  │  processTable[]  │
	  │  slotsTaken[]    │
	  └────────┬─────────┘
			   │ returns PID
			   ▼
	  ┌──────────────────┐
	  │    Scheduler     │
	  │ readyQueue[]     │
	  │ enqueue(pid)     │
	  └────────┬─────────┘
			   │ schedule()
			   ▼
		   currentPID
			   │
			   ▼
		  run process
			   │
			   ▼
	  exit_process(pid) / wait(parentPID)
```

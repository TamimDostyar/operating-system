#include "heap.h"


#define MAX_PROCESSES 1024
#define PROCESS_FREE 0
#define PROCESS_RUNNING 1
#define PROCESS_WAITING 2
#define PROCESS_STOPPED 3

// Individual process data
typedef struct {
    int processNumber;    // PID
    int processState;     // RUNNING, WAITING, STOPPED, etc.
} Process;

// Process manager
typedef struct {
    Process *processTable;      // dynamically allocated process table
    int slotsTaken[MAX_PROCESSES]; // 1 = taken, 0 = free
    int processSlot;            // current process's PID
    int totalSlots;             // total number of slots
} Pid_t;

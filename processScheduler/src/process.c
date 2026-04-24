#include "process.h"
#include "heap.h"
Pid_t pid_t;

int fork(Pid_t *pid_t, int parentPID) {
    if (pid_t->processTable == NULL) {
        Process *ptr = kmalloc(sizeof(Process) * MAX_PROCESSES);
        if (ptr == NULL) {
            return -1; // allocation failed
        }
        pid_t->processTable = ptr;
        if (pid_t->processTable == NULL) {
            return -1; // allocation failed
        }
    }

    int currentSlot = -1;

    for (int i = 2; i < MAX_PROCESSES; i++) {
        if (pid_t->slotsTaken[i] == 0) {
            currentSlot = i;
            pid_t->slotsTaken[i] = 1;
        
            pid_t->processTable[i].processNumber = i;
            pid_t->processTable[i].parentPID = parentPID;
            pid_t->processTable[i].processState = PROCESS_RUNNING;
            
            break;               
        }
    }

    if (currentSlot == -1) {
        return -1;
    }

    return currentSlot;
}


int exit_process(Pid_t *manager, int pid){
    if (pid < 2 || pid >= MAX_PROCESSES) {
        return -1;  // Invalid PID
    }
    
    if (manager->slotsTaken[pid] == 0) {
        return -1;
    }
    
    manager->slotsTaken[pid] = 0;
    
    manager->processTable[pid].processState = PROCESS_FREE;
    manager->processTable[pid].processNumber = 0;
    
    return 0;
}

int wait(Pid_t *manager, int parentPID){
    int childRunning = 1;
    
    while (childRunning) {
        childRunning = 0;        
        for (int i = 2; i < MAX_PROCESSES; i++) {
            // Check if slot is taken, is a child, and still running
            if (manager->slotsTaken[i] == 1 && 
                manager->processTable[i].parentPID == parentPID &&
                manager->processTable[i].processState == PROCESS_RUNNING) {
                childRunning = 1;
                break;  // Found a running child, stop checking
            }
        }
    }
    return 0;
}


#include <stdio.h>
#include "process.h"
#include "scheduler.h"
#include "heap.h"

int main(){
    heap_init();

    QueueScheduler scheduler;
    init_scheduler(&scheduler);
    
    Pid_t pidManager;
    pidManager.processTable = NULL;
    pidManager.processSlot = 0;
    pidManager.totalSlots = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        pidManager.slotsTaken[i] = 0;
    }
    

    int newPID = fork(&pidManager, 1);
    int newPID1 = fork(&pidManager, 1);
    
    if (newPID != -1) {

        enqueue(&scheduler, newPID);
        enqueue(&scheduler, newPID1);
        printf("Process %d created and added to queue\n", newPID);

        // dequeue(&scheduler);
        int ran = dequeue(&scheduler);
        printf("Dequeued PID: %d\n", ran);

        printf("Process %d created and added to queue\n", newPID1);
    }
    
    return 0;
}
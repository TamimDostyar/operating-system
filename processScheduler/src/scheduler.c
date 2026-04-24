#include "scheduler.h"

void init_scheduler(QueueScheduler *scheduler) {
    scheduler->queueHead = 0;
    scheduler->queueTail = 0;
    scheduler->queueSize = 0;
    scheduler->currentPID = -1;
}

int enqueue(QueueScheduler *scheduler, int pid) {
    if (scheduler->queueSize >= MAX_QUEUE_SIZE) {
        return -1; 
    }
    
    scheduler->readyQueue[scheduler->queueTail] = pid;
    scheduler->queueTail = (scheduler->queueTail + 1) % MAX_QUEUE_SIZE;
    scheduler->queueSize++;
    
    return 0;
}

int dequeue(QueueScheduler *scheduler) {
    if (scheduler->queueSize == 0) {
        return -1; 
    }
    
    int pid = scheduler->readyQueue[scheduler->queueHead];
    scheduler->queueHead = (scheduler->queueHead + 1) % MAX_QUEUE_SIZE;
    scheduler->queueSize--;
    
    return pid;
}

// Schedule next process to run
int schedule(QueueScheduler *scheduler) {
    int nextPID = dequeue(scheduler);
    
    if (nextPID == -1) {
        return -1;
    }
    
    scheduler->currentPID = nextPID;
    
    return nextPID;
}
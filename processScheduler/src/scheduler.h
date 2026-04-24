#ifndef SCHEDULER
#define SCHEDULER

#include "process.h"

#define MAX_QUEUE_SIZE 1024

typedef struct {
    int readyQueue[MAX_QUEUE_SIZE];
    int queueHead;                
    int queueTail;               
    int queueSize;              
    int currentPID;              
} QueueScheduler;

// Function declarations
void init_scheduler(QueueScheduler *scheduler);
int enqueue(QueueScheduler *scheduler, int pid);
int dequeue(QueueScheduler *scheduler);
int schedule(QueueScheduler *scheduler);

#endif
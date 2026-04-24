#include "process.h"
#include "heap.h"

Pid_t pid_t;

static inline int fork(Pid_t *pid_t) {

    if (pid_t->processTable == NULL) {
        pid_t->processTable = kmalloc(sizeof(Process) * MAX_PROCESSES);
        if (pid_t->processTable == NULL) {
            return -1; // allocation failed
        }
    }

    int currentSlot = -1;

    for (int i = 2; i < MAX_PROCESSES; i++) {
        if (pid_t->slotsTaken[i] == 0) {
            currentSlot = i;
            pid_t->slotsTaken[i] = 1;
            
            // Initialize the process
            pid_t->processTable[i].processNumber = i;
            pid_t->processTable[i].processState = PROCESS_RUNNING;
            
            break;               
        }
    }

    if (currentSlot == -1) {
        return -1; // no free PID slots available
    }

    return currentSlot;
}


static inline void exit(){
    int currentStage = pid_t.processState;
    int currentPID = pid_t.processNumber;


    // if it is running then fine
    if (currentStage == 0){
        continue;
    } else if( currentStage == 1){
        // wait like a few seconds if the program stays then terminate
        wait(2000);
        killPID(current_PID);
        // return 0;
    };
    // assiging to 0 that stage meaing it is running
    int currentStage = 0;
}



// simply exist
static void killPID(int pid){
    exit(pid);
}

// sleep
int wait(int seconds){
    sleep(seconds);
}

static inline void exec(int process){
    // since processNumber gets replaced we can exchange?
    pid_t.processNumber = process;

}


// int main(){
//     wait();
//     return 0;
// }
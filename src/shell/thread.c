
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_pthread/_pthread_t.h>
#include "common.h"

volatile int counter = 0;
int loops;

void *worker(void *args){
    int i;
    for (int i = 0; i < loops; i ++){
        counter++;
    }
    return NULL;
}

int main(int argc, char *argv[]){
    if (argc != 2){
        fprintf(stderr,"Uage: threads <value>\n");
        exit(1);
    }

    loops = atoi(argv[1]);
    pthread_t p1, p2;

    printf(&p1, NULL, worker, NULL);
    printf(&p2, NULL, worker, NULL);
    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);
    printf("Final Value: %d\n", counter);
    return 0;

}
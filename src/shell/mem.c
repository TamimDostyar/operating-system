#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include <assert.h>


int main(){
    int *p = malloc(sizeof(int));
    assert(p != NULL);

    printf("(%d) address of p:%p\n", getpid(), (void *)p);

    *p = 0;

    while (1){
        Spin(1);
        *p = *p + 1;
        printf("(%d) p: %d\n", getpid(), *p);
    }

    return 0;


}
#include <stdio.h>
#include <stdlib.h>
#include <sys/syslimits.h>
#include <unistd.h>
#include <string.h>
#include "builtins.h"

typedef struct {
    char *name;
    void (*func)(void);
} builtin_t;


// parse functions
void quitApplication(){
    printf("Exiting...\n");
    exit(0);
}
void pathDirectory(){
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd) );
    printf("%s\n", cwd);
}
void moveDirectory(){
    printf("move directory\n");
}

builtin_t builtins[] = {
    {"exit", quitApplication},
    {"cd", moveDirectory},
    {"pwd", pathDirectory},
    {NULL, NULL}
};

int is_builtin(char *cmd){
    for (int i = 0; builtins[i].name != NULL; i ++){
        if (strcmp(builtins[i].name, cmd) == 0){
            builtins[i].func();
            return 1;
        };
    }
    return 0;
}

#include <stdio.h>
#include "builtins.h"
#include <string.h>
#include "executer.h"

typedef struct {
    char *name;
    void (*func)(char *args);
} builtin_t;


builtin_t builtins[] = {
    {"exit", quitApplication},
    {"cd", computerDirectory},
    {"pwd", pathDirectory},
    {"ls", listDirectory},
    {"clear", clearShell},
    {NULL, NULL}
};

int is_builtin(char *cmd, char *args){

    for (int i = 0; builtins[i].name != NULL; i ++){
        if (strcmp(builtins[i].name, cmd) == 0){
            if (strcmp("cd", cmd) == 0) {
                builtins[i].func(args);
                return 1;
            }
            builtins[i].func(cmd);
            return 1;
        }
    }
    return 0;
}

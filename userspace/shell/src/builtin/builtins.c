#include <stdio.h>
#include "builtins.h"
#include <string.h>
#include "../executer/executer.h"

typedef struct {
    char *name;
    void (*func)(char *args);
    char *description;
} builtin_t;


builtin_t builtins[] = {
    {"exit", quitApplication, "To exit shell"},
    {"cd", computerDirectory, "To move to a directory"},
    {"pwd", pathDirectory, "To see your file path"},
    {"ls", listDirectory, "To list your files in the directory"},
    {"clear", clearShell, "To clear shell"},
    {NULL, NULL}
};

int is_builtin(char *cmd, char *args){
    // for help function
    if (cmd != NULL && strcmp(cmd, "help") == 0) {
        for (int i = 0; builtins[i].name != NULL; i++) {
            printf("%s: %s\n", builtins[i].name, builtins[i].description);
        }
        (void)args;
        return 1;
    }

    // if commands are valid then we simply provide the output
    for (int i = 0; builtins[i].name != NULL; i ++){
        if (strcmp(builtins[i].name, cmd) == 0){
            if (strcmp("cd", cmd) == 0) {
                builtins[i].func(args);
                return 1;
            }
            builtins[i].func(cmd);
            return 1;
        } 
        else if (strstr(builtins[i].name, cmd) != 0){
            printf("Not a valid command -> type help\n");
        }

    }
    return 0;
}

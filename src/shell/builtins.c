#include <stdio.h>
#include <stdlib.h>
#include <sys/syslimits.h>
#include <unistd.h>
#include <string.h>
#include "builtins.h"
#include <dirent.h>
#include <errno.h>

// global variables


typedef struct {
    char *name;
    void (*func)(char *args);
} builtin_t;

// parse functions
void quitApplication(char *args){
    printf("Exiting...\n");
    exit(0);
}
void pathDirectory(char *args){
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd) );
    printf("%s\n", cwd);
}

void computerDirectory(char *args){
    char *path = args;
    // if nothing is given it will be sent to the main root
    if (path == 0){
        path = getenv("HOME");
    }

    if (chdir(path) != 0){
        printf("cd: %s: %s\n", path, strerror(errno));
    }

    if (strcmp(path, "..") == 0){
        printf("Moving a folder back %s\n", path);
    }
    else{
        printf("Moved to %s\n", path);
    }
    printf("Type pwd to see your current path\n");

}

void listDirectory(){
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d){
        while ((dir = readdir(d)) != NULL){
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
}

builtin_t builtins[] = {
    {"exit", quitApplication},
    {"cd", computerDirectory},
    {"pwd", pathDirectory},
    {"ls", listDirectory},
    {NULL, NULL}
};

int is_builtin(char *cmd, char *args){

    for (int i = 0; builtins[i].name != NULL; i ++){
        if (strcmp(builtins[i].name, cmd) == 0){
            if (strcmp("cd", cmd) == 0){
                builtins[1].func(args);
            } else{
                builtins[i].func(cmd);
                return 1;
            }

        };
    }
    return 0;
}

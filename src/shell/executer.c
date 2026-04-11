#include <stdio.h>

#include <stdlib.h>
#include <sys/syslimits.h>
#include <unistd.h>
#include <string.h>

#include <dirent.h>
#include <errno.h>


void quitApplication(char *args){
    (void)args;
    printf("Exiting...\n");
    exit(0);
}
void pathDirectory(char *args){
    (void)args;
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd) );
    printf("%s\n", cwd);
}

void computerDirectory(char *args){
    char *path = args;
    if (path == NULL || path[0] == '\0') {
        path = getenv("HOME");
    }

    if (chdir(path) != 0){
        printf("cd: %s: %s\n", path, strerror(errno));
    }

    if (strcmp(path, "..") == 0){
        printf("Moving a folder back %s\n", path);
    }
    printf("Type pwd to see your current path\n");

}

void listDirectory(char *args){
    (void)args;
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

void clearShell(char *args){
    (void)args;
    printf("\ec");
}

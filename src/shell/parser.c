#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "builtins.h"

void parser(char *input) {
    if (input == NULL || strlen(input) == 0) {
        printf("Error: empty input\n");
        return;
    }
    is_builtin(input);
}




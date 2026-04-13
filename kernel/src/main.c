#include <stdio.h>
#include <string.h>
#include "../parser/parser.h"
// #include "history.h"

int main(void) {
    char input[1024];
    
    // condition is true 1 = true
    while (1) {
        printf("<OS>: ");
        fgets(input, sizeof(input), stdin);
        // input[strlen(input) -1] = '\0';
        input[strcspn(input, "\n")] = '\0';
        // history(input);

        if (strlen(input) > 0) {
            parser(input);
        }
    }
    return 0;
}
#include <stdio.h>
#include <string.h>
#include "parser.h"

int main(void) {
    char input[1024];
    
    // condition is true 1 = true
    while (1) {
        printf("<OS>: ");
        fgets(input, sizeof(input), stdin);
        // input[strlen(input) -1] = '\0';
        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) > 0) {
            parser(input);
        }
    }
    return 0;
}
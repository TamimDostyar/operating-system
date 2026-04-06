#include <stdio.h>
#include <string.h>

int main()
{
    // variables
    char note[100] = "Operating System Class - This OS will be written from scratch";
    int ready = 0;

    // query
    printf("Are you ready to begin? 1 for Yes and 0 for No: ");
    scanf("%d", &ready);

    if (ready == 1)
    {
        printf("Let's do it! \n");
    }
    else
    {
        printf("nah man, try again! \n");
    }

    printf("%s\n", note);

    return 0;
}
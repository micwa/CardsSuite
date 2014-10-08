#include <stdio.h>
#include <string.h>

#include "wargame.h"

int main(int argc, char *argv[])
{
    /* Start game, and hand all control to that game */
    if (argc == 1) {
        printf("No card game specified.\n");
    } else if (argc == 2) {
        if (strcmp(argv[1], "war") == 0)
        	war();
        else
        	printf("Not a valid game");
    } else {
        printf("Too many arguments!");
    }

    return 0; 
}

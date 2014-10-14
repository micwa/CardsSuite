#include <stdio.h>
#include <string.h>
#include <locale.h>

#include "wargame.h"

void game_select(char *name)
{
	printf("\n");
    if (strcmp(name, "war") == 0)
        war();
    else
       	printf("Not a valid game");
}

int main(int argc, char *argv[])
{
	setlocale(LC_CTYPE, "");
    char *games[2] = { "war", "nothing" };
    int option = 0;

    /* Start game, and hand all control to that game */
    if (argc == 1) {
        printf("No card game specified.\n");
        printf("Here is a list of included games:\n\n");
        printf("    1: War\n\n");
        printf("Choose a game: ");
        scanf("%1d", &option);
		while (getchar() != '\n')
            ;
        game_select(games[option - 1]);
    } else if (argc == 2) {
        game_select(argv[1]);
    } else {
        printf("Too many arguments!");
    }

    return 0; 
}

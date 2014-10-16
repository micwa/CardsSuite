#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

#include "wargame.h"
#include "solitgame.h"

static void game_select(char *name)
{
	printf("\n");
    if (strcmp(name, "war") == 0)
        war();
    else
       	printf("Not a valid game");
}

static void str_tolower(char *s)
{
    while (*s != '\0') {
        *s = tolower(*s);
        s++;
    }
}

int main(int argc, char *argv[])
{
	char *old_lcl = setlocale(LC_CTYPE, "en_US.utf8");
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
    	str_tolower(argv[1]);
        game_select(argv[1]);
    } else {
        printf("Too many arguments!");
    }
    setlocale(LC_CTYPE, old_lcl);
    return 0; 
}

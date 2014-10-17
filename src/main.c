#include "wargame.h"
#include "solitgame.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

#define NGAMES 2

static void game_select(char *name)
{
	printf("\n");
    if (strcmp(name, "war") == 0)
        war();
    else if (strcmp(name, "solitaire") == 0)
    	solitaire();
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
    char *games[NGAMES] = { "war", "solitaire" };
    int option = 0;

    if (argc == 1) {						/* Prompt for game if no game specified */
        printf("No card game specified.\n");
        printf("Here is a list of included games:\n\n");
        printf("    1: War\n");
        printf("    2: Solitaire\n\n");
        printf("Choose a game: ");

        do {
			scanf("%1d", &option);
			while (getchar() != '\n')
				;
        } while (option < 1 || option > NGAMES);
        game_select(games[option - 1]);
    } else if (argc == 2) {
    	str_tolower(argv[1]);				/* Launch specified game, ignoring case */
        game_select(argv[1]);
    } else
        printf("Too many arguments!");

    setlocale(LC_CTYPE, old_lcl);
    return 0; 
}

#include <stdio.h>
#include <stdlib.h>

#include "cardutility.h"
#include "gamedefs.h"
#include "warui.h"
#include "wargame.h"

void draw_war_board(const struct Player *player, const struct Player *cpu)
{
    #ifdef _WIN32                           /* First clear the board */
    system("cls");
    #else   /* Assume POSIX */
    system ("clear");
    #endif

    /* Draw cpu on top; cards in center; won cards on right; player
     * cards on bottom. Note that this assumes there is at least 1 unplayed
     * card in each player's hand. */
    struct Hand *temp;
    char *enc;
    int skipfirst = 0;						/* Skip drawing the first unplayed card */
    int index;								/* Index of that card */

    printf("\n");
    temp = cpu->hand;
    for (int i = 0; i < temp->ncards; i++) {		/* Drawing cpu hand */
    	if (!temp->isplayed[i]) {
    		if (!skipfirst) {
    		    skipfirst = 1;
    		    index = i;
    		} else
    			printf("%s   ", CARD_BACK);
    	}
    }
    skipfirst = 0;
    enc = get_card_encoding(temp->cards[index]);
    printf("\n\n\n");
    printf("%40s", enc);
    free(enc);

    printf("\n\n");
    temp = player->hand;
    for (int i = 0; i < temp->ncards; i++) {		/* Drawing player hand */
    	if (!temp->isplayed[i]) {
    		if (!skipfirst) {						/* If it's the first card, draw it */
    			skipfirst = 1;
    			enc = get_card_encoding(temp->cards[index]);
    			printf("%40s", enc);
    			printf("\n\n\n");
    			free(enc);
    		}
    		else
    			printf("%s   ", CARD_BACK);
    	}
    }
    printf("\n");
}

/* Prints game statistics */
static void print_stats()
{
	printf("Player stats:");
}

/* Start menu */
static void show_menu_start()
{
	int option;

	printf("Welcome to WAR!\n\n");
	printf("    1. Start new game\n");
	printf("    2. Resume saved game\n");
	printf("    3. Show statistics\n");
	printf("    4. Quit\n\n");

	do {
		printf("Choose an option: ");
		scanf("%d", &option);
	} while (option < 1 || option > 4);

	switch (option){
		case 1:	start_new_wargame(); 	break;
		case 2: start_saved_wargame(); 	break;
		case 3: print_stats();			break;
		case 4: quit_wargame();			break;
		default: printf("I've been hacked"); return;
	}
}

static void show_menu_pause()
{

}
static void show_menu_win()
{

}
static void show_menu_lose()
{

}

void show_war_menu(enum GameState state)
{
	split_hand(gen_random_deck(), 2);
	switch (state) {
		case START: show_menu_start();	break;
		case PAUSE: show_menu_pause(); 	break;
		case WIN: 	show_menu_win();	break;
		case LOSE: 	show_menu_lose();	break;
	}
}

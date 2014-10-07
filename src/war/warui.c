#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cardutility.h"
#include "gamedefs.h"
#include "warui.h"
#include "wargame.h"

#ifdef _WIN32
#define BOARD_CLEAR system("cls");
#define SYS_PAUSE system("ping 127.0.0.1 -n 2";
#else   /* Assume POSIX */
#define BOARD_CLEAR system("clear");		/* For some reason, also prints weird characters on screen */
#define SYS_PAUSE system("sleep 0.5");
#endif

void draw_war_board(const struct Player *player, const struct Player *cpu, const struct Card cards[])
{
    char *cpu_enc, *pl_enc, *enc, *enc2;
    int cpu_index, pl_index;				/* Index of first unplayed/number of cards unplayed */
    int cpu_todraw, pl_todraw;				/* Number of cards to draw flipped over */
    const int NUM_FRAMES = 3;

    cpu_index = cards_played(cpu->hand);
    pl_index = cards_played(player->hand);

    if (cpu_index == cpu->hand->ncards || pl_index == cpu->hand->ncards)			/* Exit if for some reason all cards played for one hand */
        	exit(EXIT_FAILURE);

    cpu_enc = get_card_encoding(&cpu->hand->cards[cpu_index]);
    pl_enc = get_card_encoding(&player->hand->cards[pl_index]);
    cpu_todraw = cpu->hand->ncards - cpu_index - 1;
    pl_todraw = player->hand->ncards - pl_index - 1;
    /* Finished initializing variables*/

    /* Draw cpu on top; cards in center; won cards on right; player
         * cards on bottom. Note that this assumes there is at least 1 unplayed
         * card in each player's hand. */
    for (int i = 0; i < NUM_FRAMES; i++) {
    	BOARD_CLEAR;                        /* First clear the board */
    	printf("\n");						/* Board top */
    	for (int i = 0; i < cpu_todraw; i++)
    		printf("%s   ", CARD_BACK);
    	printf("\n");

    	if (i != 0) {						/* Board center */
    		printf("\n");
    		if (i == 2)
    			printf("%35s", cpu_enc);
    		else
    			printf("%35s", CARD_BACK);
    	}
    	else {
    		printf("%35s", CARD_BACK);		/* Put card back closer to hand */
    		printf("\n\n");
    	}
    	if (i != 0 && cards[0].number > 0 && cpu->curr_score == 1) {
    		enc = get_card_encoding(&cards[0]);
    		enc2 = get_card_encoding(&cards[1]);
    		printf("%20s", enc); printf("%5s", enc2);
    		free(enc);
    		free(enc2);
    	}
    	printf("\n");
    	if (i != 0) {
    		if (i == 2)
    			printf("%35s", pl_enc);
    		else
    			printf("%35s", CARD_BACK);
    	}
    	if (i != 0 && cards[0].number > 0&& player->curr_score == 1) {
    		enc = get_card_encoding(&cards[0]);
    		enc2 = get_card_encoding(&cards[1]);
    		printf("%35s ", enc2); printf("%s", enc);		/* Flip order */
    		free(enc);
    		free(enc2);
    	}
    	if (i != 0)
    		printf("\n");
    	else
    		printf("%35s", CARD_BACK);

    	printf("\n");						/* Board bottom */
    	for (int i = 0; i < pl_todraw; i++)
    		printf("%s   ", CARD_BACK);
    	printf("\n");
    	if (i != NUM_FRAMES)				/* Only pause if not last "frame" */
    		SYS_PAUSE;
    }

    free(cpu_enc);
    free(pl_enc);
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
		getchar();							/* Discard newline!!! */
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
	switch (state) {
		case START: show_menu_start();	break;
		case PAUSE: show_menu_pause(); 	break;
		case WIN: 	show_menu_win();	break;
		case LOSE: 	show_menu_lose();	break;
	}
}

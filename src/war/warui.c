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
#define SYS_PAUSE system("sleep 0.1");
#endif

void draw_war_board(const struct Player_L *player, const struct Player_L *cpu, const struct Card *cards[])
{
	const int NUM_FRAMES = 3;
	int cpu_todraw, pl_todraw;				/* Number of cards to draw flipped over */
	char *cpu_enc, *pl_enc, *enc, *enc2;

	cpu_todraw = cpu->hand->ncards - 1;
	pl_todraw = player->hand->ncards - 1;
	cpu_enc = get_card_encoding(cpu->hand->node->card);
	pl_enc = get_card_encoding(player->hand->node->card);

    if (cpu_todraw == -1 || pl_todraw == -1)		/* Exit: this method does not redirect to win/lose function */
        exit(EXIT_FAILURE);

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
    	if (i != 0 && cards[0] != NULL && cpu->curr_score == 1) {
    		enc = get_card_encoding(cards[0]);
    		enc2 = get_card_encoding(cards[1]);
    		printf("%25s", enc); printf("%5s", enc2);
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
    	if (i != 0 && cards[0] != NULL && player->curr_score == 1) {
    		enc = get_card_encoding(cards[0]);
    		enc2 = get_card_encoding(cards[1]);
    		printf("%25s ", enc2); printf("%5s", enc);		/* Flip order */
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
    	if (i != NUM_FRAMES)				/* Only pause if not on last "frame" */
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
		scanf("%1d", &option);				/* Scan only one digit and */
		getchar();							/* ...discard the newline!!! */
	} while (option < 1 || option > 4);

	switch (option){
		case 1:	start_new_wargame(); 	break;
		case 2: start_saved_wargame(); 	break;
		case 3: print_stats();			break;
		case 4: quit_wargame();			break;
		default: printf("I've been hacked"); exit(EXIT_FAILURE);
	}
}

static void show_menu_pause()
{
	int option;

	printf("\n---The game has been paused---\n\n");
	printf("    1. Resume game\n");
	printf("    2. Start a new game\n");
	printf("    3. Show statistics\n");
	printf("    4. Back to main menu\n");
	printf("    5. Quit\n\n");

	do {
		printf("Choose an option: ");
		scanf("%1d", &option);				/* Scan only one digit and */
		getchar();							/* ...discard the newline!!! */
	} while (option < 1 || option > 5);

	switch (option){
		case 1:	resume_wargame();	 	break;
		case 2: start_new_wargame(); 	break;
		case 3: print_stats();			break;
		case 4: show_war_menu(START);	break;
		case 5: quit_wargame();			break;
		default: printf("I've been hacked"); exit(EXIT_FAILURE);
	}
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

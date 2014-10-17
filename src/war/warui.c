#include "cardutility.h"
#include "gamedefs.h"
#include "warui.h"
#include "wargame.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifdef _WIN32
#define BOARD_CLEAR system("cls");
#define SYS_PAUSE system("ping 1.1.1.1 -n 1 -w 100 > nul");
#else   /* Assume POSIX */
#define BOARD_CLEAR system("clear");		/* For some reason, also prints weird characters on screen */
#define SYS_PAUSE system("sleep 0.1");
#endif

void draw_war_board(const struct Player_L *player, const struct Player_L *cpu, const struct Card *cards[])
{
	const int NUM_FRAMES = 3;
	extern char *card_encs[52];
	int cpu_todraw, pl_todraw;				/* Number of cards to draw flipped over */
	char *cpu_enc, *pl_enc;

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
    	for (int j = 0; j < cpu_todraw; j++)
    		printf("%s   ", CARD_BACK);
    	printf("\n");

    	if (i != 0) {						/* Board center */
    		printf("\n");
    		if (i == 2)
    			printf("%25s", cpu_enc);
    		else
    			printf("%25s", CARD_BACK);
    	}
    	else {
    		printf("%25s", CARD_BACK);	/* Put card back closer to hand */
    		printf("\n\n");
    	}
    	if (i != 0 && cards[0] != NULL && cpu->curr_score == 1) {
    		printf("%25s", card_encs[get_card_value(cards[0])]);
    		printf("%5s", card_encs[get_card_value(cards[1])]);
    	}
    	printf("\n\n");
    	if (i != 0) {
    		if (i == 2)
    			printf("%25s", pl_enc);
    		else
    			printf("%25s", CARD_BACK);
    	}
    	if (i != 0 && cards[0] != NULL && player->curr_score == 1) {
    		printf("%25s", card_encs[get_card_value(cards[1])]);	/* Flip order */
    		printf("%5s", card_encs[get_card_value(cards[0])]);
    	}
    	if (i != 0)
    		printf("\n");
    	else
    		printf("%25s", CARD_BACK);

    	printf("\n");						/* Board bottom */
    	for (int j = 0; j < pl_todraw; j++)
    		printf("%s   ", CARD_BACK);
    	printf("\n");
    	if (i != NUM_FRAMES)				/* Only pause if not on last "frame" */
    		SYS_PAUSE;
    }
    free(cpu_enc);
    free(pl_enc);
}

/* "Safe" scanf() for one digit only */
static void getopt(int *option)
{
	scanf("%1d", option);
	while (getchar() != '\n')
		;
}

/* Prints game statistics */
static void print_stats()
{
	extern const char *WAR_STATS;
	int cpu_wins = 0, cpu_losses = 0, p_wins = 0, p_losses = 0;
	double cpu_perc = 0, p_perc = 0;
	FILE *file = fopen(WAR_STATS, "r");
	if (file != NULL) {
		fscanf(file, "%9d %9d", &cpu_wins, &cpu_losses);
		while (fgetc(file) != '\n')
			;
		fscanf(file, "%9d %9d", &p_wins, &p_losses);

		/* Percentage-calculating time! */
		if (cpu_wins > 0 && cpu_losses == 0)
			cpu_perc = 100;
		else
			cpu_perc = cpu_wins * 100.0 / (cpu_wins + cpu_losses);

		if (p_wins > 0 && p_losses == 0)
			p_perc = 100;
		else
			p_perc = p_wins * 100.0 / (p_wins + p_losses);
		fclose(file);
	}

	printf("------------------------------\n\n");
	printf("Player stats:\n\n");
	printf("  CPU wins: %d\n", 		cpu_wins);
	printf("  CPU losses: %d\n", 	cpu_losses);
	printf("  CPU Win%%: %d%%\n\n",		(int)cpu_perc);
	printf("  Player wins: %d\n", 	p_wins);
	printf("  Player losses: %d\n", p_losses);
	printf("  Player Win%%: %d%%\n\n",	(int)p_perc);
	printf("------------------------------\n\n");
}

/* Start menu */
static void show_menu_start()
{
	int option = 0;

	printf("Welcome to WAR!\n\n");
	printf("    1. Start new game\n");
	printf("    2. Resume saved game\n");
	printf("    3. Show statistics\n");
	printf("    4. Quit\n\n");

	do {
		printf("Choose an option: ");
		getopt(&option);
	} while (option < 1 || option > 4);

	switch (option){
		case 1:	start_new_wargame(); 	break;
		case 2: start_saved_wargame(); 	break;
		case 3: print_stats(); show_war_menu();	break;
		case 4: quit_wargame();			break;
		default: printf("I've been hacked\n"); exit(EXIT_FAILURE);
	}
}

static void show_menu_pause()
{
	int option = 0;

	printf("\n---The game has been paused---\n\n");
	printf("    1. Resume game\n");
	printf("    2. Start a new game\n");
	printf("    3. Save current game\n");
	printf("    4. Show statistics\n");
	printf("    5. Quit\n\n");

	do {
		printf("Choose an option: ");
		getopt(&option);
	} while (option < 1 || option > 5);

	switch (option){
		case 1:	resume_wargame();	 	break;
		case 2: start_new_wargame(); 	break;
		case 3: save_wargame();	show_war_menu(); 	break;
		case 4: print_stats(); show_war_menu();		break;
		case 5: quit_wargame();			break;
		default: printf("I've been hacked\n"); exit(EXIT_FAILURE);
	}
}
static void show_menu_win()
{
	int option = 0;

	printf("\nCONGRATULATIONS on hacking your way to victory! ");
	printf("If you did win legitimately, however, either a) you killed your Enter key or b) your computer does not know how to generate random numbers.");
	printf("\n\nPlay again?\n");
	printf("    1. Start a new game\n");
	printf("    2. Show statistics\n");
	printf("    3. Quit\n");

	do {
		printf("Choose an option: ");
		getopt(&option);
	} while (option < 1 || option > 3);

	switch (option){
		case 1: start_new_wargame(); 	break;
		case 2: print_stats(); show_war_menu();	break;
		case 3: quit_wargame();			break;
		default: printf("I've been hacked\n"); exit(EXIT_FAILURE);
	}
}

static void show_menu_lose()
{
	int option = 0;

	printf("\nCONGRATULATIONS on somehow losing! ");
	printf("I don't know why you did it, but you beat the odds and lost!");
	printf("\n\nPlay again?\n");
	printf("    1. Start a new game\n");
	printf("    2. Show statistics\n");
	printf("    3. Quit\n");

	do {
		printf("Choose an option: ");
		getopt(&option);
	} while (option < 1 || option > 3);

	switch (option){
	case 1: start_new_wargame(); 	break;
	case 2: print_stats(); show_war_menu();	break;
	case 3: quit_wargame();			break;
	default: printf("I've been hacked\n"); exit(EXIT_FAILURE);
	}
}

void show_war_menu()
{
	extern enum GameState war_curr_state;

	switch (war_curr_state) {
		case START: show_menu_start();	break;
		case PAUSE: show_menu_pause(); 	break;
		case WIN: 	show_menu_win();	break;
		case LOSE: 	show_menu_lose();	break;
	}
}

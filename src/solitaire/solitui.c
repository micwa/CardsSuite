#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "solitui.h"
#include "solitgame.h"
#include "cardutility.h"
#include "gamedefs.h"

void draw_solit_board(int *waste_index, int only_rows, int only_cols)
{
	return;
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
	extern const char *SOLIT_STATS;
	int wins, losses, score;
	FILE *file = fopen(SOLIT_STATS, "r");
	if (file != NULL) {
		fclose(file);
	} else {
		printf("No stats file found. Save the game. ")
	}

	printf("------------------------------\n\n");
	printf("Player stats:\n\n");
	printf("------------------------------\n\n");
}

/* Start menu */
static void show_menu_start()
{
	int option = 0;

	printf("Let's play some solitaire!\n\n");
	printf("    1. Start new game\n");
	printf("    2. Resume saved game\n");
	printf("    3. Show statistics\n");
	printf("    4. Quit\n\n");

	do {
		printf("Choose an option: ");
		getopt(&option);
	} while (option < 1 || option > 4);

	switch (option){
		case 1:	start_new_solitgame(); 		break;
		case 2: start_saved_solitgame(); 	break;
		case 3: print_stats(); show_solit_menu();	break;
		case 4: quit_solitgame();			break;
		default: printf("I've been hacked"); exit(EXIT_FAILURE);
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
		case 1:	resume_solitgame();	 	break;
		case 2: start_new_solitgame(); 	break;
		case 3: save_solitgame(); show_solit_menu(); 	break;
		case 4: print_stats(); show_solit_menu();		break;
		case 5: quit_solitgame();		break;
		default: printf("I've been hacked\n"); exit(EXIT_FAILURE);
	}
}
static void show_menu_win()
{
	int option = 0;

	printf("\nCONGRATULATIONS on winning! ");
	printf("\n\nPlay again?\n");
	printf("    1. Start a new game\n");
	printf("    2. Show statistics\n");
	printf("    3. Quit\n");

	do {
		printf("Choose an option: ");
		getopt(&option);
	} while (option < 1 || option > 3);

	switch (option){
		case 1: start_new_solitgame(); 	break;
		case 2: print_stats(); show_solit_menu();	break;
		case 3: quit_solitgame();		break;
		default: printf("I've been hacked\n"); exit(EXIT_FAILURE);
	}
}

void show_solit_menu()
{
	extern enum GameState solit_curr_state;

	switch (solit_curr_state) {
		case START: show_menu_start();	break;
		case PAUSE: show_menu_pause(); 	break;
		case WIN: 	show_menu_win();	break;
		case LOSE: 	printf("You can't lose this game\n"); exit(EXIT_FAILURE);
	}
}

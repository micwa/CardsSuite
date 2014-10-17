#include "solitui.h"
#include "solitgame.h"
#include "cardutility.h"
#include "carddefs.h"
#include "gamedefs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifdef _WIN32
#define BOARD_CLEAR system("cls");
#else   /* Assume POSIX */
#define BOARD_CLEAR system("clear");
#endif

void draw_solit_board(int waste_index, int tbl_first[7], int draw_rows, int draw_cols)
{
	extern struct Hand *stock_hand;
	extern struct LinkedHand *tbl_hand[7];
	extern struct Card *fdtion_top[4];
	extern char *card_encs[52];
	char *fdtion_encs[4] = { "   ", "   ", "   ", "   " };	/* Print three spaces if no card on foundation pile */
	int max_rows = 0;
	BOARD_CLEAR;

	/* Drawing the board:
	 * - each row has 12 initial spaces (two for row alphabet)
	 * - each card or placeholder spaces must be three characters wide
	 * - there is one blank line after each tableau row */

	/* Top row of cards */
	printf("\n");
	if (waste_index < 0) {
		printf("%12s", " ");
		printf("%s    ", CARD_BACK);		/* CARD_BACK followed by four spaces or by the waste card */
	} else {
		printf("%12s", " ");
		printf("%s %s ", CARD_BACK, card_encs[get_card_value(&stock_hand->cards[waste_index])]);
	}
	for (int i = 0; i < 4; i++)
		if (fdtion_top[i] != NULL)
			fdtion_encs[i] = card_encs[get_card_value(fdtion_top[i])];
	printf("%s %s %s %s \n", fdtion_encs[0], fdtion_encs[1], fdtion_encs[2], fdtion_encs[3]);

	/* Column stwxyz + numbers */
	printf("%12s", " "); printf("s  t  "); printf("%3s", " "); printf("w  x  y  z  \n\n");
	printf("%12s", " ");
	for (int i = 0; i < 7; i++)
		printf("%d  ", i + 1);
	printf("\n");

	/* The rows now */
	for (int i = 0; i < 7; i++)				/* First determine max_rows */
		if (tbl_hand[i]->ncards > max_rows)
			max_rows = tbl_hand[i]->ncards;
	for (int i = 0; i < max_rows; i++) {
		printf("%10s", " ");
		printf("%c ", 'a' + i);				/* Alphabet */
		for (int j = 0; j < 7; j++) {
			if (tbl_hand[j]->ncards < i + 1)		/* Print spaces if past # of cards in pile */
				printf("   ");
			else {
				if (tbl_first[j] > i)
					printf("%s  ", CARD_BACK);
				else {
					int val = get_card_value(linked_hand_get_card(tbl_hand[j], i));
					printf("%s  ", card_encs[val]);
				}
			}
		}
		printf("\n\n");
	}
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
	int wins = 0, losses = 0, score = 0;
	FILE *file = fopen(SOLIT_STATS, "r");
	if (file != NULL) {
		fclose(file);
	} else
		printf("No stats file found.\n");

	printf("------------------------------\n\n");
	printf("Player stats:\n\n");
	printf("    Wins: %d\n", wins);
	printf("    Losses: %d\n", losses);
	printf("    Lowest number of moves: %d\n", score);
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

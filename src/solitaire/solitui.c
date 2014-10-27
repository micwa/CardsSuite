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
	char *fdtion_encs[4] = { " ", " ", " ", " " };	/* Print spaces if no card on foundation pile */
	int max_rows = 0;

	/* Drawing the board:
	 * - each row has 12 initial spaces (two for row alphabet)
	 * - each card or placeholder spaces must be three characters wide
	 * - there is one blank line after each tableau row */
	BOARD_CLEAR;

	/* Top row of cards */
	printf("\n");
	if (waste_index < 0) {
		printf("%12s", " ");
		printf("%s     ", CARD_BACK);		/* CARD_BACK followed by four spaces or by the waste card */
	} else {
		printf("%12s", " ");
		printf("%s  %s  ", CARD_BACK, g_card_encs[get_card_value(&g_stock_hand->cards[waste_index])]);
	}
	for (int i = 0; i < 4; i++)
		if (g_fdtion_top[i]->number > 0)
			fdtion_encs[i] = g_card_encs[get_card_value(g_fdtion_top[i])];
	printf("%3s", " ");
	printf("%s  %s  %s  %s  \n", fdtion_encs[0], fdtion_encs[1], fdtion_encs[2], fdtion_encs[3]);

	/* Topmost column, and column numbers */
	printf("%12s", " "); printf("s  t  "); printf("%3s", " "); printf("w  x  y  z  \n\n");
	if (draw_cols) {
		printf("%12s", " ");
		for (int i = 0; i < 7; i++)
			printf("%d  ", i + 1);
		printf("\n");
	}

	/* The rows now */
	for (int i = 0; i < 7; i++)				/* First determine max_rows */
		if (g_tbl_hand[i]->ncards > max_rows)
			max_rows = g_tbl_hand[i]->ncards;
	for (int i = 0; i < max_rows; i++) {
		if (draw_rows) {
			printf("%10s", " ");
			printf("%c ", 'a' + i);	        /* Alphabet */
		} else
			printf("%12s", " ");

		for (int j = 0; j < 7; j++) {
			if (g_tbl_hand[j]->ncards < i + 1)		/* Print spaces if past # of cards in pile */
				printf("%3s", " ");
			else {
				if (tbl_first[j] > i)
					printf("%s  ", CARD_BACK);
				else if (tbl_first[j] == -1)
					printf("%3s", " ");
				else {
					int val = get_card_value(linked_hand_get_card(g_tbl_hand[j], i));
					printf("%s  ", g_card_encs[val]);
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
	int wins = 0, losses = 0, score = 0;
	FILE *file = fopen(SOLIT_STATS, "r");
	if (file != NULL) {
		fclose(file);
	} else
		printf("\nNo stats file found.\n");

	printf("\n---------PLAYER STATS---------\n\n");
	printf("  Wins: %d\n", wins);
	printf("  Losses: %d\n", losses);
	printf("  Lowest number of moves: %d\n", score);
	printf("\n------------------------------\n\n");

	printf("Press <Enter> to continue...\n");
	while (getchar() != '\n')
			;
}

/* Start menu */
static PTFV show_menu_start()
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
		case 1:	return &start_new_solitgame;
		case 2: return &start_saved_solitgame;
		case 3: return &print_stats;
		case 4: return &quit_solitgame;
		default: printf("I've been hacked"); exit(EXIT_FAILURE);
	}
}

static PTFV show_menu_pause()
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
		case 1:	return &resume_solitgame;
		case 2: return &start_new_solitgame;
		case 3: return &save_solitgame;
		case 4: return &print_stats;
		case 5: return &quit_solitgame;
		default: printf("I've been hacked\n"); exit(EXIT_FAILURE);
	}
}
static PTFV show_menu_win()
{
	int option = 0;

	printf("\nCONGRATULATIONS on winning!");
	printf("\n\nPlay again?\n\n");
	printf("    1. Start a new game\n");
	printf("    2. Show statistics\n");
	printf("    3. Quit\n\n");

	do {
		printf("Choose an option: ");
		getopt(&option);
	} while (option < 1 || option > 3);

	switch (option){
		case 1: return &start_new_solitgame;
		case 2: return &print_stats;
		case 3: return &quit_solitgame;
		default: printf("I've been hacked\n"); exit(EXIT_FAILURE);
	}
}

PTFV show_solit_menu()
{
	switch (g_solit_curr_state) {
		case START: return show_menu_start();
		case PAUSE: return show_menu_pause();
		case WIN: 	return show_menu_win();
		default: printf("I've been hacked\n"); exit(EXIT_FAILURE);
	}
}

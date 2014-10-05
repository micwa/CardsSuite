#include <stdio.h>

#include "wargame.h"

static enum GameState curr_state = START;

void show_war_menu()
{
	switch (curr_state) {
		case START: show_menu_start();	break;
		case PAUSE: show_menu_paused(); break;
		case WIN: show_menu_win();		break;
		case LOSE: show_menu_lose();	break;
	}
}

static void show_menu_start()
{
	printf("Welcome to WAR!\n\n");
	printf("\t1. Start new game\n");
	printf("\t2. Resume saved game\n");
	printf("\t3. Show statistics\n");
	printf("\t4. Quit\n\n");
	printf("\tChoose an option: ");
}

static void start_war_game()
{
	show_war_menu();
}

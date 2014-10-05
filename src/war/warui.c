#include <stdio.h>

#include "cardutility.h"
#include "warui.h"
#include "wargame.h"

static void print_stats()
{
	printf("Player stats:");
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

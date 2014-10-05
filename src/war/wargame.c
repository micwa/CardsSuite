#include <stdio.h>
#include <stdlib.h>

#include "wargame.h"
#include "warui.h"
#include "carddefs.h"
#include "cardutility.h"

const char * STATS_FILE = "player.stats";
const char * SAVE_FILE = "player.save";

static enum GameState curr_state = START;
static struct Hand *player_hand, *cpu_hand;

void quit_wargame()
{
	printf("Thank you for playing!");
	free(player_hand->cards);
	free(cpu_hand->cards);
}

void save_wargame()
{

}

void start_new_wargame()
{
	printf("New game started\n");
	struct Hand *h = split_hand(gen_random_deck(), 2);
	player_hand = &h[0];
	cpu_hand = &h[1];

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < h[i].ncards; j++) {
			printf("%s ", get_card_encoding(h[i].cards[j]));
		}
		printf("\n");
	}
}

void start_saved_wargame()
{
	printf("Saved game started\n");
}

void war()
{
	printf("WAR\n");
	show_war_menu(curr_state);
}

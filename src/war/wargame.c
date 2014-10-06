#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wargame.h"
#include "warui.h"
#include "carddefs.h"
#include "cardutility.h"
#include "gamedefs.h"

const char *STATS_FILE = "player.stats";
const char *SAVE_FILE = "player.save";

static enum GameState curr_state = START;
static struct Player *player = NULL, *cpu = NULL;

static void players_destroy()
{
	struct Hand *temp;
    if (player != NULL) {
    	temp = player->hand;
        free(temp->cards);
        free(temp->isplayed);
        free(temp);
        free(player);
    }
    if (cpu != NULL) {
    	temp = cpu->hand;
    	free(temp->cards);
    	free(temp->isplayed);
    	free(temp);
    	free(cpu);
    }
}

static void play_game()
{
    char c;
    draw_war_board(player, cpu);
    
    while (curr_state == START) {
        /* Prompt user to press enter, and skip over all input */
        printf("Press enter to turn over the next card (or p to pause): ");
        while ((c = getchar()) != '\n') {
            if (c == 'p')
                curr_state = PAUSE;
        }
        
        /* Check if all cards are gone; if so, refill hand or show win/lose.
         * Then "flip" the card by setting the next isplayed[i] to 1 */
        if (all_cards_played(player->hand)) {
        	printf("Refilling hand...\n");
        }
    }
}

void quit_wargame()
{
	printf("Thank player for playing! Bye!\n");
    players_destroy();
}

void save_wargame()
{

}

void start_new_wargame()
{
	printf("Starting new game...\n");
    players_destroy();
    
    /* Set up players, then play */
    player = malloc(sizeof(struct Player));
	struct Hand *h = split_hand(gen_random_deck(), 2);
	player->hand = &h[0];
	player->nwins = 0; player->nlosses = 0;
	player->curr_score = 0;
	cpu->hand = &h[1];
	cpu->nwins = 0; cpu->nlosses = 0;
	cpu->curr_score = 0;
    
    play_game();

	/*for (int i = 0; i < 2; i++) {
		for (int j = 0; j < h[i].ncards; j++) {
			printf("%s ", get_card_encoding(h[i].cards[j]));
		}
		printf("\n");
	}*/
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

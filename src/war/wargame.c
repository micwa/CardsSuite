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

/* Start playing the game, assuming that hands are initialized */
static void play_game()
{
	/* For comparing current cards, and storing previous ones for drawing.
	 * cpu is index 0, player is index 1 */
	struct Hand *temp;
	struct Card *ccur[2] = { NULL, NULL }, *cprev[2] = { NULL, NULL };
    char c;

    draw_war_board(player, cpu, cprev);
    
    while (curr_state == START) {

        /* Prompt user to press enter, and skip over all input */
        printf("Press 'n' to turn over the next card (or p to pause):");
        while ((c = getchar()) != '\n') {
            if (c == 'p')
                curr_state = PAUSE;
        }

        /* Find next unplayed card, and "flip" the card by setting the
         * next isplayed[i] to 1. */
        temp = cpu->hand;
        for (int i = 0; i < temp->ncards; i++) {
        	if (!temp->isplayed) {
        		cprev[0] = ccur[0];
        		ccur[0] = &temp->cards[i];
        		temp->isplayed[i] = 1;
        		break;
        	}
        }
        temp = player->hand;
        for (int i = 0; i < temp->ncards; i++) {
        	if (!temp->isplayed) {
        		cprev[1] = ccur[1];
        		ccur[1] = &temp->cards[i];
        		temp->isplayed[i] = 1;
        		break;
        	}
        }

        /* Compare hands (so warui.c can draw them properly) */
        if (card_compare(ccur[0], ccur[1]) > 0) {
        	cpu->curr_score = 1;
        	player->curr_score = 0;
        } else {
        	cpu->curr_score = 0;
        	player->curr_score = 1;
        }

        /* Check if all cards are gone; if so, refill hand or show win/lose. */
        if (all_cards_played(player->hand)) {
        	printf("Refilling hand...\n");
        }
    }
}

/* Frees all malloc'd memory (players, hands, etc.*/
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

void quit_wargame()
{
	printf("Thank you for playing. Bye!\n");
    players_destroy();
}

void save_wargame()
{

}

void start_new_wargame()
{
	printf("Starting new game...\n");
    players_destroy();						/* If quit previous game */
    
    /* Set up players, then play */
    struct Hand deck = gen_random_deck();
    struct Hand *h = split_hand(&deck, 2);
    player = malloc(sizeof(struct Player));
    cpu = malloc(sizeof(struct Player));

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
	show_war_menu(curr_state);
}

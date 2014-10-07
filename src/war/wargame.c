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
static struct Card ccur[2] = { {-1, -1},  {-1, -1} };		/* The played cards */
static struct Card *cpu_acc[52] = { NULL }, *pl_acc[52] = { NULL };	/* The won cards */
static int acc_count[2] = { 0, 0 };

/* Destroys the given hand (and its members) */
static void hand_destroy(struct Hand *hand)
{
	free(hand->cards);
	free(hand->isplayed);
	free(hand);
}

/* Start playing the game, assuming that hands are initialized */
static void play_game()
{
	/* For comparing current cards, and storing previous ones for drawing.
	 * cpu is index 0, player is index 1 */
	struct Hand *temp;
    int cpu_played, pl_played; 				/* Number of cards unplayed */
    char c;
    
    while (curr_state == START) {
    	draw_war_board(player, cpu, (const struct Card **)ccur);	/* Since settings of ccur[] is after this */

        /* Prompt user to press enter, and skip over all input */
        printf("Press enter to turn over the next card (or p to pause):");
        while ((c = getchar()) != '\n') {
            if (c == 'p')
                curr_state = PAUSE;
        }

        /* Find next unplayed card, and "flip" the card by setting the
         * next isplayed[i] to 1. */
        cpu_played = cards_played(cpu->hand);
        pl_played = cards_played(player->hand);

        ccur[0] = cpu->hand->cards[cpu_played];				/* COPY the card so that the number can be set to -1 later */
        cpu->hand->isplayed[cpu_played] = 1;
        ccur[1] = player->hand->cards[pl_played];
        player->hand->isplayed[pl_played] = 1;

        /* Compare hands (so warui.c can draw them properly) and exchange won/lost cards*/
        if (card_compare(&ccur[0], &ccur[1]) > 0) {			/* CPU wins */
        	cpu->curr_score = 1;
        	player->curr_score = 0;
        	player->hand->cards[pl_played].number = -1;			/* Set lost card to -1, and give "lost" card to cpu */
        	cpu_acc[acc_count[0]++] = &player->hand->cards[pl_played];
        } else {
        	cpu->curr_score = 0;
        	player->curr_score = 1;
        	cpu->hand->cards[cpu_played].number = -1;
        	pl_acc[acc_count[1]++] = &cpu->hand->cards[cpu_played];
        }

        /* Check if all cards are gone; if so, refill hand. */
        if (cards_played(cpu->hand) == cpu->hand->ncards) {
        	printf("Refilling cpu hand...\n");
        	temp = cpu->hand;
        	cpu->hand = squash_hands(cpu->hand, cpu_acc);
        	hand_destroy(temp);				/* Make sure previous hand was destroyed */
        	acc_count[0] = 0;				/* Reset the acc_count and accumulator card stack */
        	for (int i = 0; i < 52; i++)
        		cpu_acc[i] = NULL;
        }
        if (cards_played(player->hand) == player->hand->ncards) {
        	printf("Refilling player hand...\n");
        	temp = player->hand;
        	player->hand = squash_hands(player->hand, pl_acc);
        	hand_destroy(temp);
        	acc_count[1] = 0;
        	for (int i = 0; i < 52; i++)
        		pl_acc[i] = NULL;
        }
    }
}

/* Frees all malloc'd memory (players, hands, etc.*/
static void players_destroy()
{
    if (player != NULL) {
    	hand_destroy(player->hand);
        free(player);
    }
    if (cpu != NULL) {
    	hand_destroy(cpu->hand);
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
    struct Hand deck = gen_random_deck(1000);
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

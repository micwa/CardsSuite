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
const int SHUFFLE_AMOUNT = 1000;

static enum GameState curr_state = START;
static struct Player_L *player = NULL, *cpu = NULL;
static int nturns = 0;						/* Turns played so far */

/* Set the current state to PAUSE and bring up the pause menu */
static void pause_game()
{
	curr_state = PAUSE;
	show_war_menu(curr_state);
}

/* Start playing the game, assuming that hands are initialized */
static void play_game()
{
	/* For comparing current cards, and storing previous ones for drawing.
	 * cpu is index 0, player is index 1 */
	struct Card *ccur[2] = { NULL, NULL };	/* The played cards */
	struct CardNode *temp;
    int c;
    
    while (curr_state == START) {
    	draw_war_board(player, cpu, (const struct Card **)ccur);	/* Since settings of ccur[] is after this */

        /* Prompt user to press enter, and skip over all input */
    	nturns++;
    	printf("\n---TURN %d---\n", nturns);
    	printf("CPU's cards: %d\n", cpu->hand->ncards);
    	printf("Your cards: %d\n\n", player->hand->ncards);
        printf("Press enter to turn over the next card (or p to pause):");
        while ((c = getchar()) != '\n') {
            if (c == 'p')
                pause_game();
        }

        ccur[0] = cpu->hand->node->card;
        ccur[1] = player->hand->node->card;

        /* Compare hands (so warui.c can draw them properly)
         * and exchange won/lost cards */
        if (card_compare(ccur[0], ccur[1]) > 0) {
        	cpu->curr_score = 1;			/* Set rightmost bit to 1 for win, 0 for loss */
        	player->curr_score = 0;

        	/* Unlink node from losing player, add it to winning player,
        	 * flip cards for both players. Don't create a cycle! */
        	temp = player->hand->node;
        	player->hand->node = player->hand->node->next;	/* Do this before the next line, or the node loses its "next" pointer */
        	temp->next = NULL;
        	linked_hand_add(cpu->hand, temp);

        	temp = cpu->hand->node;
        	cpu->hand->node = cpu->hand->node->next;
        	temp->next = NULL;
        	linked_hand_add(cpu->hand, temp);
        	cpu->hand->ncards += 1;
        	player->hand->ncards -= 1;
        } else {
        	cpu->curr_score = 0;
        	player->curr_score = 1;

        	temp = cpu->hand->node;
        	cpu->hand->node = cpu->hand->node->next;
        	temp->next = NULL;
        	linked_hand_add(player->hand, temp);

        	temp = player->hand->node;
        	player->hand->node = player->hand->node->next;
        	temp->next = NULL;
        	linked_hand_add(player->hand, temp);
        	player->hand->ncards += 1;
        	cpu->hand->ncards -= 1;
        }
    }
}

/* Frees all malloc'd memory (players, hands, etc.*/
static void players_destroy()
{
    if (player != NULL) {
    	free_linked_hand(player->hand);
        free(player);
    }
    if (cpu != NULL) {
    	free_linked_hand(cpu->hand);
    	free(cpu);
    }
}

void quit_wargame()
{
	printf("Thank you for playing. Bye!\n");
    players_destroy();
}

void resume_wargame()
{
	play_game();							/* An alias for play_game() (for now), essentially */
}

void save_wargame()
{

}

void start_new_wargame()
{
	printf("Starting new game...\n");
    players_destroy();						/* If quitting previous game */
    
    /* Set up players, then play */
    struct Hand deck = gen_ordered_deck();
    shuffle_hand(&deck, SHUFFLE_AMOUNT);
    struct Hand *hands = split_hand(&deck, 2);
    struct LinkedHand *l_cpu, *l_player;

    cpu = malloc(sizeof(struct Player));
    player = malloc(sizeof(struct Player));
    l_cpu = malloc(sizeof(struct LinkedHand));		/* Per player, its LinkedHand and its node MUST be initialized */
    l_player = malloc(sizeof(struct LinkedHand));	/* (Or else fill_linked_hand()/free() will crash) */
    l_cpu->node = malloc(sizeof(struct CardNode));
    l_player->node = malloc(sizeof(struct CardNode));

    fill_linked_hand(l_cpu, &hands[0]);
    fill_linked_hand(l_player, &hands[1]);
    free(hands);							/* Don't need it anymore */

	cpu->hand = l_cpu;
	cpu->nwins = 0; cpu->nlosses = 0;
	cpu->curr_score = 0;
	player->hand = l_player;
	player->nwins = 0; player->nlosses = 0;
	player->curr_score = 0;
    
	nturns = 0;
    play_game();
}

void start_saved_wargame()
{
	printf("Saved game started\n");
}

void war()
{
	show_war_menu(curr_state);
}

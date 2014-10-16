#include <stdio.h>
#include <stdlib.h>

#include "wargame.h"
#include "warui.h"
#include "carddefs.h"
#include "cardutility.h"
#include "gamedefs.h"

const char *WAR_STATS = "war.stats";
const char *WAR_SAVE = "war.deck";
static const int SHUFFLE_AMOUNT = 1000;

enum GameState war_curr_state = START;
static struct Player_L *player = NULL, *cpu = NULL;
static struct Card *ccur[2] = { NULL, NULL };	    /* The played cards */
static int nturns = 0;						/* Turns played so far */
static struct Hand *curr_hand = NULL;		/* For memory leak purposes only */
static int opened_hand = 0;

static void fload_stats()
{
	FILE *file = fopen(WAR_STATS, "r");
	if (file == NULL) {						/* Default: set everything to 0 */
		cpu->nwins = 0;
		cpu->nlosses = 0;
		cpu->curr_score = 0;
		player->nwins = 0;
		player->nlosses = 0;
		player->curr_score = 0;
	} else {
		fscanf(file, "%9d %9d %9d", &cpu->nwins, &cpu->nlosses, &cpu->curr_score);		/* Overflow protection */
		while (fgetc(file) != '\n')
			;
		fscanf(file, "%9d %9d %9d", &player->nwins, &player->nlosses, &player->curr_score);
		fclose(file);
	}
}

/* Writes to the stats file the players' stats, one player per line terminating
 * with a newline character. Returns 1 on success, -1 on failure */
static int fsave_stats()
{
	FILE *file = fopen(WAR_STATS, "w");
	if (file == NULL)
		return -1;

	fprintf(file, "%d %d %d\n", cpu->nwins, cpu->nlosses, cpu->curr_score);
	fprintf(file, "%d %d %d\n", player->nwins, player->nlosses, player->curr_score);
	fclose(file);
	return 1;
}

/* Set the current state to PAUSE and bring up the pause menu */
static void pause_game()
{
	war_curr_state = PAUSE;
	show_war_menu();
}

/* Start playing the game, assuming that hands are initialized */
static void play_game()
{
	/* For comparing current cards, and storing previous ones for drawing.
	 * cpu is index 0, player is index 1 */
	struct CardNode *temp;
    int c;
    
    while (war_curr_state == START) {
    	draw_war_board(player, cpu, (const struct Card **)ccur);	/* Since setting of ccur[] is after this */

        /* Prompt user to press enter, and skip over all input */
    	printf("\n---TURN %d---\n", nturns);
    	printf("CPU's cards: %d\n", cpu->hand->ncards);
    	printf("Your cards: %d\n\n", player->hand->ncards);
        printf("Press enter to turn over the next card (or p to pause):");
        while ((c = getchar()) != '\n') {
            if (c == 'p')
                pause_game();
        }
        nturns++;

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

        /* If somehow the player won or lost, save stats, show the appropriate menu */
        if (player->hand->ncards == 0) {
        	war_curr_state = LOSE;
        	fsave_stats();
        	show_war_menu();
        } else if (player->hand->ncards == 52) {
        	war_curr_state = WIN;
        	fsave_stats();
        	show_war_menu();
        }
    }
}

/* Frees all malloc'd memory (players, hands, etc.) */
static void players_destroy()
{
	if (cpu != NULL) {
		free_linked_hand(cpu->hand, 0);
		free(cpu);
	}
    if (player != NULL) {
    	free_linked_hand(player->hand, 0);
        free(player);
    }
    if (curr_hand != NULL) {
    	if (opened_hand) {						/* Solution for now: if fopen_hand(), curr_hand */
    		free_hand(&curr_hand[0], 0);		/* is just an array; else, was malloc'd */
    		free_hand(&curr_hand[1], 0);
    		free(curr_hand);
    	}
    	else
    		free_hand(curr_hand, 1);
    }
}

/* Allocates memory for the players, their LinkedHands, and the first CardNode */
static void players_init()
{
	cpu = malloc(sizeof(struct Player_L));
	player = malloc(sizeof(struct Player_L));
	cpu->hand = malloc(sizeof(struct LinkedHand));			/* For each player, its LinkedHand and its node MUST be initialized */
	player->hand = malloc(sizeof(struct LinkedHand));		/* (Or else fill_linked_hand()/free() will crash) */
	cpu->hand->node = NULL;
	player->hand->node = NULL;
}

void quit_wargame()
{
	printf("Thank you for playing. Bye!\n");
    players_destroy();

    exit(EXIT_SUCCESS);
}

void resume_wargame()
{
	war_curr_state = START;
	play_game();							/* An alias for play_game(), essentially (at least for now) */
}

void save_wargame()
{
	FILE *file = fopen(WAR_SAVE, "w");
	if (file == NULL) {
		printf("Error writing save file: returning\n");
		return;
	}
	/* Save the turn number */
	fprintf(file, "%d\n", nturns);

	fsave_linked_hand(cpu->hand, file);
	fsave_linked_hand(player->hand, file);
    if (fsave_stats() < 0) {
        printf("Error saving player stats: returning\n");
        return;
    }
    fclose(file);
    printf("\nSuccessfully saved game!\n\n");
}

void start_new_wargame()
{
	printf("Starting new game...\n");
    players_destroy();						/* If quitting previous game */
    
    /* Set up LinkedHands, then play */
    struct Hand *deck = gen_ordered_deck();
    shuffle_hand(deck, SHUFFLE_AMOUNT);
    curr_hand = split_hand(deck, 2);

    /* Initialize players, load stats */
    players_init();
    fload_stats();
    fill_linked_hand(cpu->hand, &curr_hand[0]);
    fill_linked_hand(player->hand, &curr_hand[1]);
    opened_hand = 0;
    free(curr_hand);						/* Because we don't need this anymore */
    curr_hand = deck;
    
	nturns = 0;
	war_curr_state = START;
    play_game();
}

void start_saved_wargame()
{
	players_destroy();

	int turns = 0;
	FILE *file = fopen(WAR_SAVE, "r");
	if (file == NULL) {
		printf("No save file found, or error opening file.\n");
		show_war_menu();
		return;
	}
	/* Get the turn number first */
	fscanf(file, "%9d", &turns);
	while (fgetc(file) != '\n')
		;
	curr_hand = fopen_hand(file, 2);
	fclose(file);

	players_init();
	fload_stats();
	fill_linked_hand(cpu->hand, &curr_hand[0]);
	fill_linked_hand(player->hand, &curr_hand[1]);
	opened_hand = 1;

	nturns = turns;
	war_curr_state = START;
	play_game();
}

void war()
{
	show_war_menu();
}

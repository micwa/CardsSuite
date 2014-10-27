#include "wargame.h"
#include "warui.h"
#include "carddefs.h"
#include "gamedefs.h"
#include "cardutility.h"
#include "structhelper.h"
#include "fileops.h"

#include <stdio.h>
#include <stdlib.h>

#define SHUFFLE_AMOUNT 1000

const char *WAR_STATS = "war.stats";
const char *WAR_SAVE = "war.deck";
enum GameState g_war_curr_state = START;

static struct Player_L *player = NULL, *cpu = NULL;
static struct Card *ccur[2] = { NULL, NULL };	    /* The played cards */
static int nturns = 0;						/* Turns played so far */

static void fload_stats()
{
	FILE *file = fopen(WAR_STATS, "r");
	if (file == NULL) {						/* Default: set everything to 0 */
		playerl_init(cpu, 0, 0, 0);
		playerl_init(player, 0, 0, 0);
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

/* Frees all malloc'd memory (players, hands, etc.) */
static void game_destroy()
{
	if (cpu != NULL) {
		free_linked_hand(cpu->hand, 1);
		free(cpu);
	}
    if (player != NULL) {
    	free_linked_hand(player->hand, 1);
        free(player);
    }
    ccur[0] = NULL;
    ccur[1] = NULL;
}

/* Sets up the players, load stats, and set up the LinkedHands */
static void game_init()
{
	cpu = malloc(sizeof(struct Player_L));
	player = malloc(sizeof(struct Player_L));
	fload_stats();

	cpu->hand = linked_hand_create();
	player->hand = linked_hand_create();
    
    init_card_encs();
}

/* Lose the game */
static void lose_game()
{
	g_war_curr_state = LOSE;
	fsave_stats();
}

/* Set the current state to PAUSE */
static void pause_game()
{
	g_war_curr_state = PAUSE;
}

/* Win the game */
static void win_game()
{
	g_war_curr_state = WIN;
	fsave_stats();
}

/* Start playing the game, assuming that hands are initialized.
 * Returns on pause_game(), win_game(), and lose_game(). */
static void play_game()
{
	/* For comparing current cards, and storing previous ones for drawing.
	 * cpu is index 0, player is index 1 */
	struct CardNode *temp;
    int c;
    
    while (g_war_curr_state == START) {
    	draw_war_board(player, cpu, (const struct Card **)ccur);	/* Since setting of ccur[] is after this */

        /* Prompt user to press enter, and skip over all input */
    	printf("\n---TURN %d---\n", nturns);
    	printf("CPU's cards: %d\n", cpu->hand->ncards);
    	printf("Your cards: %d\n\n", player->hand->ncards);
        printf("Press enter to turn over the next card (or p to pause):");
        while ((c = getchar()) != '\n')
            if (c == 'p') {
                pause_game();
                return;
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
        	temp = linked_hand_remove(player->hand, 0, 0);
        	temp->next = NULL;				/* Set next to NULL before adding to cpu's hand */
        	linked_hand_add(cpu->hand, temp);

        	temp = linked_hand_remove(cpu->hand, 0, 0);
        	temp->next = NULL;
        	linked_hand_add(cpu->hand, temp);
        } else {
        	cpu->curr_score = 0;
        	player->curr_score = 1;

        	temp = linked_hand_remove(cpu->hand, 0, 0);
        	temp->next = NULL;
        	linked_hand_add(player->hand, temp);

        	temp = linked_hand_remove(player->hand, 0, 0);
        	temp->next = NULL;
        	linked_hand_add(player->hand, temp);
        }

        /* If somehow the player won or lost, save stats, show the appropriate menu */
        if (player->hand->ncards == 0) {
        	lose_game();
        	return;							/* Not necessary, but for clarity */
        }
        else if (player->hand->ncards == 52) {
        	win_game();
        	return;
        }
    }
}

void quit_wargame()
{
	printf("Thank you for playing. Bye!\n");
    game_destroy();
    free_card_encs();

    exit(EXIT_SUCCESS);
}

void resume_wargame()
{
	g_war_curr_state = START;
	play_game();							/* Only function other than start_*_wargame() that should call play_game() */
}

void save_wargame()
{
	FILE *file = fopen(WAR_SAVE, "w");
	if (file == NULL) {
		printf("Error opening save file for write: returning\n");
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
    game_destroy();							/* If quitting previous game */
    
    /* Set up LinkedHands, then play */
    struct Hand *deck = gen_ordered_deck(), **split;
    shuffle_hand(deck, SHUFFLE_AMOUNT);
    split = split_hand(deck, 2);

    /* Initialize game, load stats */
    game_init();
    fill_linked_hand(cpu->hand, split[0]);
    fill_linked_hand(player->hand, split[1]);
    free_hand(deck, 1);						/* Free all used Hands */
    for (int i = 0; i < 2; i++)
    	free_hand(split[i], 1);
    free(split);
    
	nturns = 0;
	g_war_curr_state = START;
    play_game();
}

void start_saved_wargame()
{
	game_destroy();

	int turns = 0;
    struct Hand **hands;
	FILE *file = fopen(WAR_SAVE, "r");
	if (file == NULL) {
		printf("No save file found, or error opening file.\n\n");
		return;
	}
	/* Get the turn number first */
	fscanf(file, "%9d", &turns);
	while (fgetc(file) != '\n')
		;
	hands = fopen_hand(file, 2);
	fclose(file);

	game_init();
	fill_linked_hand(cpu->hand, hands[0]);
	fill_linked_hand(player->hand, hands[1]);
    for (int i = 0; i < 2; i++)
        free_hand(hands[i], 1);
    free(hands);

	nturns = turns;
	g_war_curr_state = START;
	play_game();
}

void war()
{
	while (1) {								/* Infinite loop until quit_wargame() is called */
		PTFV fptr = show_war_menu();
		(*fptr)();
	}
}

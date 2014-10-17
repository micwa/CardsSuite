#include "solitgame.h"
#include "solitui.h"
#include "solitlogic.h"
#include "carddefs.h"
#include "cardutility.h"
#include "structhelper.h"
#include "gamedefs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHUFFLE_AMOUNT 1000
#define NCARDS_STOCK 24

const char *SOLIT_STATS = "solit.stats";
const char *SOLIT_SAVE = "solit.deck";

enum GameState solit_curr_state = START;
enum MoveType solit_curr_move = NONE;
struct Hand *stock_hand = NULL;
struct LinkedHand *tbl_hand[7] = { NULL };
struct Card *fdtion_top[4] = { NULL };   	/* No point in creating an array/otherwise for the foundation cards */

/* tbl_first[i] is the index of the first card in tbl_hand[i] to be drawn face up, where [0] = card 1, etc.
 * tbl_first[i] must be in the range (0...ncards-1), unless it equals -1, in which case all cards are face down */
static int tbl_first[7];
static struct Player *player = NULL;        /* Primarily for storing original hand; also keeps track of stats */
static int nmoves;						    /* Moves made so far */
static int waste_index;                     /* Index of the *current* card shown in the waste pile */
static int is_encs_init = 0;

static void fload_stats()
{
	FILE *file = fopen(SOLIT_STATS, "r");
	if (file == NULL) {						/* Default: set everything to 0 */
		player->nwins = 0;
		player->nlosses = 0;
		player->curr_score = 0;
	} else {
		fscanf(file, "%9d %9d %9d", &player->nwins, &player->nlosses, &player->curr_score);
		fclose(file);
	}
}

/* Writes to the stats file the player's stats in one line, terminating
 * with a newline character. Returns 1 on success, -1 on failure */
static int fsave_stats()
{
	FILE *file = fopen(SOLIT_STATS, "w");
	if (file == NULL)
		return -1;
        
	fprintf(file, "%d %d %d\n", player->nwins, player->nlosses, player->curr_score);
	fclose(file);
	return 1;
}

/* Frees all malloc'd memory (players, hands, etc.) */
static void game_destroy()
{
    if (player != NULL) {
    	free_hand(player->hand, 1);
        free(player);
    }
    /* free() everything */
    if (stock_hand != NULL)
    	free_hand(stock_hand, 1);
    for (int i = 0; i < 7; i++)
        if (tbl_hand[i] != NULL)
            free_linked_hand(tbl_hand[i], 0);
}

/* Allocates memory for the player, sets its Hand to NULL (other initialization for
 * the player is done in fload_stats()). Also allocates memory for ALL Hands/LinkedHands
 * & members (one level down). */
static void game_init()
{
	player = malloc(sizeof(struct Player));
	player->hand = NULL;
	fload_stats();
    
	/* Initialize all hands; for LinkedHands, make their CardNodes NULL */
	stock_hand = hand_create(NCARDS_STOCK);
    for (int i = 0; i < 7; i++)
    	tbl_hand[i] = linked_hand_create();

    if (!is_encs_init) {					/* Initialize card encodings */
    	init_card_encs();
    	is_encs_init = 1;
    }
}

/* Set the current state to PAUSE and bring up the pause menu */
static void pause_game()
{
	solit_curr_state = PAUSE;
	show_solit_menu();
}

/* Start playing the game, assuming that hands are initialized.
 * Most logic checking/move making is "outsourced" to solitutil. */
static void play_game()
{
    int option;
    
    while (solit_curr_state == START) {
    	draw_solit_board(waste_index, tbl_first, 1, 1);

    	/* Get input */
    	printf("Make a move: ");
    	scanf("%1d", &option);
    	while (getchar() != '\n')
    		;

        /* Show menu if won
        if (solit_game_win()) {
        	solit_curr_state = WIN;
        	fsave_stats();
        	show_solit_menu();
        }*/
    }
}

void quit_solitgame()
{
	printf("Thank you for playing. Bye!\n");
    game_destroy();
    free_card_encs();

    exit(EXIT_SUCCESS);
}

void resume_solitgame()
{
	solit_curr_state = START;
	play_game();							/* An alias for play_game(), essentially (at least for now) */
}

void save_solitgame()
{
	/*FILE *file = fopen(SOLIT_SAVE, "w");
	if (file == NULL) {
		printf("Error writing save file: returning\n");
		return;
	}
	fprintf(file, "%d\n", nturns);

	fsave_linked_hand(cpu->hand, file);
	fsave_linked_hand(player->hand, file);
    if (fsave_stats() < 0) {
        printf("Error saving player stats: returning\n");
        return;
    }
    fclose(file);*/
    printf("\nSuccessfully saved game!\n\n");
}

void start_new_solitgame()
{
	printf("Starting new game...\n");
    game_destroy();
    
    struct CardNode *temp;
    int count = 0;							/* For counting the index of the generated deck */
    struct Hand *deck = gen_ordered_deck();      	/* The player keeps the original hand with malloc'd hand/cards (and isplayed, but we don't use that) */
    shuffle_hand(deck, SHUFFLE_AMOUNT);
    
    /* Initialize game, then fill up hands */
    game_init();
    /* Note that Hand copies the cards from the deck; the LinkedHands do not */
    for (count = 0; count < NCARDS_STOCK; count++)
    	stock_hand->cards[count] = deck->cards[count];
    for (int i = 0; i < 7; i++) {
    	for (int j = 0; j < i + 1; j++) {
    		temp = malloc(sizeof(struct CardNode));
    		temp->card = &deck->cards[count++];
    		temp->next = NULL;
    		linked_hand_add(tbl_hand[i], temp);
    	}
    	tbl_hand[i]->ncards = i + 1;
    	tbl_first[i] = i;                   /* Positions 0 to 6 */
    }
    player->hand = deck;
    
	nmoves = 0;
	waste_index = -1;
	solit_curr_state = START;
    play_game();
}

void start_saved_solitgame()
{
	game_destroy();

	int moves = 0;
	/*FILE *file = fopen(WAR_SAVE, "r");
	if (file == NULL) {
		printf("No save file found, or error opening file.\n");
		show_war_menu();
		return;
	}
	Get the move number first
	fscanf(file, "%9d", &turns);
	while (fgetc(file) != '\n')
		;
	curr_hand = fopen_hand(file, 2);
	fclose(file);

	players_init();
	fload_stats();
	fill_linked_hand(cpu->hand, &curr_hand[0]);
	fill_linked_hand(player->hand, &curr_hand[1]);
	opened_hand = 1; */

	nmoves = moves;
	solit_curr_state = START;
	play_game();
}

void solitaire()
{
	show_solit_menu();
}

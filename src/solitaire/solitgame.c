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
enum GameState g_solit_curr_state = START;
struct Hand *g_stock_hand = NULL;
struct LinkedHand *g_tbl_hand[7] = { NULL };
struct Card *g_fdtion_top[4] = { NULL };   	/* No point in creating an array/otherwise for the foundation cards */

/* tbl_first[i] is the index of the first card in tbl_hand[i] to be drawn face up, where [0] = card 1, etc.
 * tbl_first[i] must be in the range (0...ncards-1), unless it equals -1, in which case all cards are face down */
static int tbl_first[7];
static struct Player *player = NULL;        /* Primarily for storing original hand; also keeps track of stats */
static int nmoves = 0;						/* Moves made so far */
static int waste_index = -1;                /* Index of the *current* card shown in the waste pile */
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
    if (g_stock_hand != NULL)
    	free_hand(g_stock_hand, 1);
    for (int i = 0; i < 7; i++)
        if (g_tbl_hand[i] != NULL)
            free_linked_hand(g_tbl_hand[i], 0);
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
	g_stock_hand = hand_create(NCARDS_STOCK);
    for (int i = 0; i < 7; i++)
    	g_tbl_hand[i] = linked_hand_create();
    for (int i = 0; i < 4; i++)
    	g_fdtion_top[i] = NULL;

    if (!is_encs_init) {					/* Initialize card encodings */
    	init_card_encs();
    	is_encs_init = 1;
    }
}

static void getopt(char *option)
{
	scanf("%1c", option);
	while (getchar() != '\n')
		;
}

/* Set the current state to PAUSE and bring up the pause menu */
static void pause_game()
{
	g_solit_curr_state = PAUSE;
	show_solit_menu();
}

/* Start playing the game, assuming that hands are initialized.
 * Most logic checking/move making is "outsourced" to solitutil. */
static void play_game()
{
    char option;
    int max_rows = 0, error_flag = 0, stock_empty_flag = 0;
    int col, col2, row;						/* The src/dest columns, when needed; row is only for TBL_TO_TBL */
    enum MoveType curr_move = NONE;
    struct Card *card_src = NULL, *card_dest = NULL;
    void *src = NULL, *dest = NULL;	        /* src = either current waste, tableau hand, or tableau node */
    										/* dest = either tbl, tbl_empty, fdtion */
    while (g_solit_curr_state == START) {
    	draw_solit_board(waste_index, tbl_first, 1, 1);

    	/* Get input */
        if (error_flag) {
            printf("Not a valid move. Try again.\n");
            error_flag = 0;
        }
        if (stock_empty_flag)
            printf("There are no more cards in the stock.\n");
        printf("\n---MOVE %d---\n", nmoves);
    	printf("Make a move (p for pause): ");
    	getopt(&option);
          
        for (int i = 0; i < 7; i++)         /* Find max rows */
            if (g_tbl_hand[i]->ncards > max_rows)
                max_rows = g_tbl_hand[i]->ncards;
         
        if (option == 'p') {
            curr_move = NONE;
            pause_game();
        } else if (option == 's' && !stock_empty_flag) {
            curr_move = FLIP_STOCK;
            waste_index = get_next_unplayed(g_stock_hand, waste_index + 1, 1);		/* Get the next unplayed index, and show that card next */

            if (waste_index == -1 && nmoves > 0)    /* Set stock_empty flag if waste_index is -1, and more than one move's been made */
                stock_empty_flag = 1;
        } else if (option == 't') {
            /* First, check if the waste_index is valid. Then possibles moves from here are:
             * move to tableau, tableau_empty, or a foundation pile */
            if (waste_index == -1)
                goto error;
            draw_solit_board(waste_index, tbl_first, 0, 1);
            printf("Select a column or foundation pile: ");
            getopt(&option);

            if (option >= '1' && option <= '7') {
            	if (g_tbl_hand[option - '1']->ncards == 0)	/* If the tableau pile is empty */
            		curr_move = WASTE_TO_TBL_EMPTY;
            	else
            		curr_move = WASTE_TO_TBL;
            	col = option - '1';
            	card_src = &g_stock_hand->cards[waste_index];
            	card_dest = linked_hand_get_card(g_tbl_hand[option - '1'],	/* May be NULL */
            			g_tbl_hand[col]->ncards - 1);
            	src = (void *) card_src;
            	dest = (void *) g_tbl_hand[col];
            } else if (option >= 'w' && option <= 'z') {
            	col = option - 'w';
            	curr_move = WASTE_TO_FDTION;
            	card_src = &g_stock_hand->cards[waste_index];
            	card_dest = g_fdtion_top[col];
            	src = (void *) card_src;
            	dest = (void *) &g_fdtion_top[col];
            } else
            	goto error;
        } else if (option >= '1' && option <= '7') {
            /* To move a single card, select a column (1-7) or a foundation pile (w, x, y, z).
             * To move many cards, select a row AND THEN a column */
        	struct CardNode *node;
            col = option - '1';
            draw_solit_board(waste_index, tbl_first, 1, 1);
            printf("Select a column, foundation pile, or row: ");
            getopt(&option);
            row = option - 'a';				/* Only used when moving multiple cards */
            
            if (option >= '1' && option <= '7') {
                col2 = option - '1';
                row = g_tbl_hand[col]->ncards - 1;
                if (g_tbl_hand[col2]->ncards == 0)
                	curr_move = TBL_TO_TBL_EMPTY;
                else
                	curr_move = TBL_TO_TBL;

                card_src = linked_hand_get_card(g_tbl_hand[col], row);
                card_dest = linked_hand_get_card(g_tbl_hand[col2], g_tbl_hand[col2]->ncards - 1);
                node = linked_hand_get_node(g_tbl_hand[col], row);
                src = (void *) node;
                dest = (void *) g_tbl_hand[col2];
            } else if (option >= 'w' && option <= 'z') {
            	row = g_tbl_hand[col]->ncards - 1;
                curr_move = TBL_SINGLE_TO_FDTION;
                card_src = linked_hand_get_card(g_tbl_hand[col], g_tbl_hand[col]->ncards - 1);
                card_dest = g_fdtion_top[option - 'w'];
                src = (void *) card_src;
                dest = (void *) &g_fdtion_top[option - 'w'];
            } else if (row >= tbl_first[col] &&	row < g_tbl_hand[col]->ncards) {
            	/* Prompt for column */
            	draw_solit_board(waste_index, tbl_first, 0, 1);
            	printf("Select a column: ");
            	getopt(&option);

            	if (option >= '1' && option <= '7') {
            		col2 = option - '1';
            		if (g_tbl_hand[col2]->ncards == 0)
            			curr_move = TBL_TO_TBL_EMPTY;
            		else
            			curr_move = TBL_TO_TBL;

            		card_src = linked_hand_get_card(g_tbl_hand[col], row);
            		card_dest = linked_hand_get_card(g_tbl_hand[col2], g_tbl_hand[col2]->ncards - 1);
            		node = linked_hand_get_node(g_tbl_hand[col], row);
            		src = (void *) node;
            		dest = (void *) g_tbl_hand[col2];
            	} else
            		goto error;
            } else
            	goto error;
        } else {
error:
            error_flag = 1;
            continue;
        }

        /* Now making the move, and removing/modifying indices */
        if (is_valid_move(curr_move, card_src, card_dest) == 1) {
        	make_move(curr_move, src, dest);

        	switch (curr_move) {
        		case WASTE_TO_TBL_EMPTY:	/* Only case where col = destination column */
        			tbl_first[col] = 0;		/* Set to 0 since the pile now has one card */
        		case WASTE_TO_TBL:
        		case WASTE_TO_FDTION:
        			g_stock_hand->isplayed[waste_index] = 1;
        			waste_index = get_next_unplayed(g_stock_hand, waste_index + 1, 1);
        			break;
        		case TBL_TO_TBL_EMPTY:		/* Don't need to remove nodes since removed already */
        			tbl_first[col2] = 0;
        		case TBL_TO_TBL:
        		case TBL_SINGLE_TO_FDTION:
        			/* If the resulting amount of cards - 1 is less than the previous tbl_first[i],
        			 * then set to the last card (essentially flip it over). */
        			if (g_tbl_hand[col]->ncards <= tbl_first[col])
        				tbl_first[col] = g_tbl_hand[col]->ncards - 1;
        			break;
        		case FLIP_STOCK:			/* Don't need to do anything */
        		case NONE:
        			break;
        		case TBL_TO_FDTION:
        		default:
        			printf("Not even possible.\n");
        			exit(EXIT_FAILURE);
        	}
        } else
        	goto error;
        nmoves++;
        
        /* On win game */
        if (solit_game_win(g_fdtion_top)) {
        	g_solit_curr_state = WIN;
        	fsave_stats();
        	show_solit_menu();
        }
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
	g_solit_curr_state = START;
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
    
    int count = 0;							/* For counting the index of the generated deck */
    struct Hand *deck = gen_ordered_deck();      	/* The player keeps the original hand with malloc'd hand/cards (and isplayed, but we don't use that) */
    shuffle_hand(deck, SHUFFLE_AMOUNT);
    
    /* Initialize game, then fill up hands */
    game_init();

    for (count = 0; count < NCARDS_STOCK; count++)	/* Note that Hand copies the cards from the deck; the LinkedHands do not */
    	g_stock_hand->cards[count] = deck->cards[count];
    for (int i = 0; i < 7; i++) {
    	for (int j = 0; j < i + 1; j++)
    		linked_hand_add(g_tbl_hand[i], card_node_create(&deck->cards[count++], NULL));

    	/* Don't need to set g_tbl_hand[i]->ncards since add()ing it sets it already */
    	tbl_first[i] = i;                   /* Positions 0 to 6 */
    }
    player->hand = deck;
    
	nmoves = 0;
	waste_index = -1;
	g_solit_curr_state = START;
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
	g_solit_curr_state = START;
	play_game();
}

void solitaire()
{
	show_solit_menu();
}

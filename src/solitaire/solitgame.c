#include "solitgame.h"
#include "solitdefs.h"
#include "solitui.h"
#include "solitlogic.h"
#include "solitstack.h"
#include "carddefs.h"
#include "gamedefs.h"
#include "cardutility.h"
#include "structhelper.h"
#include "fileops.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NCARDS_STOCK 24
#define SHUFFLE_AMOUNT 1000
#define STACK_SIZE 10

const char *SOLIT_STATS = "solit.stats";
const char *SOLIT_SAVE = "solit.deck";
enum GameState g_solit_curr_state = START;
struct Hand *g_stock_hand = NULL;
struct LinkedHand *g_tbl_hand[7] = { NULL };
struct Card *g_fdtion_top[4] = { NULL };    /* No point in creating an array/otherwise for the foundation cards */

/* tbl_first[i] is the index of the first card in tbl_hand[i] to be drawn face up, where [0] = card 1, etc.
 * tbl_first[i] must be in the range (0...ncards-1), unless it equals -1, in which case all cards are face down */
static int tbl_first[7];
static struct SolitStack *move_stack = NULL;
static struct Player *player = NULL;        /* Used ONLY for tracking stats */
static int nmoves = 0;                      /* Moves made so far */
static int waste_index = -1;                /* Index of the *current* card shown in the waste pile */

static void fload_stats()
{
    FILE *file = fopen(SOLIT_STATS, "r");
    if (file == NULL) {                     /* Default: set everything to 0 */
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
    
    /* Write losses even though it has no meaning */
    fprintf(file, "%d %d %d\n", player->nwins, player->nlosses, player->curr_score);
    fclose(file);
    return 1;
}

/* Frees all malloc'd memory (players, hands, etc.) */
static void game_destroy()
{
    if (player != NULL)
        free(player);
    if (move_stack != NULL)
        stack_destroy(move_stack);

    /* free() all hands */
    if (g_stock_hand != NULL)
        free_hand(g_stock_hand, 1);
    for (int i = 0; i < 7; i++)
        if (g_tbl_hand[i] != NULL)
            free_linked_hand(g_tbl_hand[i], 1);
    for (int i = 0; i < 4; i++)             /* Cards are always malloc()'d */
        if (g_fdtion_top[i] != NULL)
            free(g_fdtion_top[i]);
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
    for (int i = 0; i < 4; i++) {
        g_fdtion_top[i] = malloc(sizeof(struct Card));
        g_fdtion_top[i]->number = -1;       /* Set illogical values */
        g_fdtion_top[i]->suit = 5;
    }
    move_stack = stack_create(STACK_SIZE);
    init_card_encs();
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
}

/* Win the game */
static void win_game()
{
    g_solit_curr_state = WIN;
    fsave_stats();
}

/* Start playing the game, assuming that hands are initialized.
 * Most logic checking/move making is "outsourced" to solitlogic. */
static void play_game()
{
    char option;
    int max_rows = 0, error_flag = 0, stock_empty_flag = 0, undos_empty_flag = 0;
    int col_src, col_dest, row;             /* The src/dest columns, when needed; row is only for TBL_TO_TBL */
    struct SolitMove *curr_move = malloc(sizeof(struct SolitMove));

    while (g_solit_curr_state == START) {

        draw_solit_board(waste_index, tbl_first, 1, 1);

        /* Display errors, and then get input */
        if (undos_empty_flag)
            printf("No more undoes left.\n");
        if (error_flag) {
            printf("Not a valid move. Try again.\n");
            error_flag = 0;
        }

        if (stock_empty_flag)
            printf("There are no more cards in the stock.\n");
        printf("\n---MOVE %d---\n", nmoves);
        printf("Make a move (P for pause): ");
        getopt(&option);
          
        for (int i = 0; i < 7; i++)         /* Find max rows */
            if (g_tbl_hand[i]->ncards > max_rows)
                max_rows = g_tbl_hand[i]->ncards;
         
        if (option == 'P') {
            curr_move->type = NONE;
            pause_game();
            free(curr_move);
            return;
        } else if (option == 'u') {
            /* Pop the last move off the stack, call undo_move(), and change the indices/
             * remove the nodes as appropriate */
            struct SolitMove *move = stack_pop(move_stack);
            struct LinkedHand *temp_hand;
            struct CardNode *temp_node;
            struct Card *temp_card, *temp_card2;
            int num = move->num % 100;      /* The "base" number, for easy reference */

            if (move == NULL) {
                undos_empty_flag = 1;
                continue;
            }

            switch (move->type) {           /* Add cards/set LinkedHands back to their original state before undoing */
                case FLIP_STOCK:
                    waste_index = num;
                    if (waste_index != -1)
                        g_stock_hand->isplayed[waste_index] = 0;
                    undo_move(move);                /* Undo (does nothing) */
                    break;
                case WASTE_TO_TBL_EMPTY:
                    tbl_first[move->num/100 - 1] = -1;
                case WASTE_TO_FDTION:
                    waste_index = num;
                    g_stock_hand->isplayed[waste_index] = 0;
                    undo_move(move);                /* Undo */
                    break;
                case WASTE_TO_TBL:
                    waste_index = num;
                    g_stock_hand->isplayed[waste_index] = 0;
                    temp_node = (struct CardNode *)undo_move(move); /* Undo, and free hand-less node */
                    free(temp_node->card);
                    free(temp_node);
                    break;
                case TBL_TO_TBL_EMPTY:
                    tbl_first[move->num/100 - 1] = -1;
                case TBL_TO_TBL:
                    temp_node = (struct CardNode *)undo_move(move); /* Undo, and re-add to the LinkedHand */
                    temp_hand = g_tbl_hand[move->num/1000 - 1];
                    linked_hand_add(temp_hand, temp_node);

                    /* Revert to previous tbl_first[i] */
                    tbl_first[move->num%1000/100 - 1] = move->num/1000 - 1;
                    break;
                case TBL_SINGLE_TO_FDTION:
                    temp_card2 = (struct Card *)undo_move(move);    /* Undo, and create new card for the LinkedHand (create new node) */
                    temp_card = malloc(sizeof(struct Card));
                    temp_card->number = temp_card2->number + 1;
                    temp_card->suit = temp_card2->suit;       /* Kind of cheating, but only way because src card was free()'d already */
                    linked_hand_add(g_tbl_hand[move->num%1000/100 - 1], card_node_create(temp_card, NULL));

                    tbl_first[move->num%1000/100 - 1] = move->num/1000 - 1;
                    break;
                case NONE:                  /* Not add-able to the stack */
                case TBL_TO_FDTION:
                default:
                    printf("Not even possible.\n");
                    exit(EXIT_FAILURE);
            }
            /* Go to the next turn */
            continue;
        } else if (option == 's' && !stock_empty_flag) {
            curr_move->type = FLIP_STOCK;
            curr_move->num = waste_index;
        } else if (option == 't') {
            /* First, check if the waste_index is valid. Then possibles moves from here are:
             * move to tableau, tableau_empty, or a foundation pile */
            if (waste_index == -1)
                goto error;
            draw_solit_board(waste_index, tbl_first, 0, 1);
            printf("Select a column or foundation pile: ");
            getopt(&option);

            if (option >= '1' && option <= '7') {
                if (g_tbl_hand[option - '1']->ncards == 0)    /* If the tableau pile is empty */
                    curr_move->type = WASTE_TO_TBL_EMPTY;
                else
                    curr_move->type = WASTE_TO_TBL;
                col_dest = option - '1';
                curr_move->src = (void *) &g_stock_hand->cards[waste_index];
                curr_move->dest = (void *) g_tbl_hand[col_dest];
                curr_move->num = waste_index + (col_dest + 1) * 100;        /* Encoded so that col_to is hundreds digit */
            } else if (option >= 'w' && option <= 'z') {
                col_dest = option - 'w';
                curr_move->type = WASTE_TO_FDTION;
                curr_move->src = (void *) &g_stock_hand->cards[waste_index];
                curr_move->dest = (void *) g_fdtion_top[col_dest];
                curr_move->num = waste_index + (col_dest + 1) * 100;
            } else
                goto error;
        } else if (option >= '1' && option <= '7') {
            /* To move a single card, select a column (1-7) or a foundation pile (w, x, y, z).
             * To move many cards, select a row AND THEN a column */
            col_src = option - '1';
            if (tbl_first[col_src] < 0)     /* Error if no cards in pile */
                goto error;

            draw_solit_board(waste_index, tbl_first, 1, 1);
            printf("Select a column, foundation pile, or row: ");
            getopt(&option);
            row = option - 'a';             /* Only used when moving multiple cards */

            if (option >= 'w' && option <= 'z') {
                col_dest = option - 'w';
                row = g_tbl_hand[col_src]->ncards - 1;
                curr_move->type = TBL_SINGLE_TO_FDTION;
                curr_move->src = (void *) linked_hand_get_card(g_tbl_hand[col_src], g_tbl_hand[col_src]->ncards - 1);
                curr_move->dest = (void *) g_fdtion_top[col_dest];
                /* Encode the src column AND the previous tbl_first[i] */
                curr_move->num = (col_src + 1) * 100 + (tbl_first[col_src] + 1) * 1000;
            } else {
                /* Prompt for column if alphabet character was typed */
                if (row >= tbl_first[col_src] && row < g_tbl_hand[col_src]->ncards) {
                    draw_solit_board(waste_index, tbl_first, 0, 1);
                    printf("Select a column: ");
                    getopt(&option);
                } else        /* Set the proper row (last one) */
                    row = g_tbl_hand[col_src]->ncards - 1;
                /* TBL_SINGLE and TBL (many) _TO_TBL are handled the same way */
                if (option >= '1' && option <= '7') {
                    col_dest = option - '1';
                    if (g_tbl_hand[col_dest]->ncards == 0)
                        curr_move->type = TBL_TO_TBL_EMPTY;
                    else
                        curr_move->type = TBL_TO_TBL;

                    curr_move->src = (void *) linked_hand_get_node(g_tbl_hand[col_src], row);
                    curr_move->dest = (void *)g_tbl_hand[col_dest];
                    /* Number of cards moved, src column, previous tbl_first[i] */
                    curr_move->num = g_tbl_hand[col_src]->ncards - row + (col_src + 1) * 100 + (tbl_first[col_src] + 1) * 1000;
                } else
                    goto error;
            }
        } else {
error:
            error_flag = 1;
            continue;
        }

        /* Now make the move, push it onto the stack, and do ALL the processing here
         * (i.e. removing nodes, changing waste_index/tbl_first[i]) */
        if (is_valid_move(curr_move) == 1) {
            make_move(curr_move);
            stack_push(move_stack, curr_move);
            undos_empty_flag = 0;           /* Reset flag for undoes here */

            switch (curr_move->type) {
                struct CardNode *tofree;

                case WASTE_TO_TBL_EMPTY:
                    tbl_first[col_dest] = 0;        /* Set to 0 since the pile now has one card */
                case WASTE_TO_TBL:
                case WASTE_TO_FDTION:
                    g_stock_hand->isplayed[waste_index] = 1;
                    waste_index = get_next_unplayed(g_stock_hand, waste_index + 1, 1);
                    break;
                case TBL_TO_TBL_EMPTY:
                    tbl_first[col_dest] = 0;
                case TBL_TO_TBL:
                    /* If the resulting amount of rows - 1 is less than the previous tbl_first[i],
                     * then set to the last card (essentially flip it over). */
                    linked_hand_remove(g_tbl_hand[col_src], row, 1);
                    if (g_tbl_hand[col_src]->ncards - 1 < tbl_first[col_src])
                        tbl_first[col_src] = g_tbl_hand[col_src]->ncards - 1;
                    break;
                case TBL_SINGLE_TO_FDTION:
                    tofree = linked_hand_remove(g_tbl_hand[col_src], g_tbl_hand[col_src]->ncards - 1, 1);
                    free(tofree->card);
                    free(tofree);
                    if (g_tbl_hand[col_src]->ncards - 1 < tbl_first[col_src])
                        tbl_first[col_src] = g_tbl_hand[col_src]->ncards - 1;
                    break;
                case FLIP_STOCK:
                    /* Get the next unplayed index, and show that card next */
                    waste_index = get_next_unplayed(g_stock_hand, waste_index + 1, 1);
                    if (waste_index == -1)          /* Set stock_empty flag if no more cards in stock */
                        stock_empty_flag = 1;
                    break;
                case NONE:
                case TBL_TO_FDTION:
                default:
                    printf("Not even possible.\n");
                    exit(EXIT_FAILURE);
            }
        } else
            goto error;
        nmoves++;
        
        /* On win game */
        if (solit_game_win((const struct Card **)g_fdtion_top)) {
            if (nmoves < player->curr_score || player->curr_score == 0)
                player->curr_score = nmoves;
            win_game();
            free(curr_move);
            return;
        }
    }
    free(curr_move);
}

void quit_solitgame()
{
    printf("\nThank you for playing. Bye!\n");
    game_destroy();
    free_card_encs();

    exit(EXIT_SUCCESS);
}

void resume_solitgame()
{
    g_solit_curr_state = START;
    play_game();
}

void save_solitgame()
{
    int vals[4] = { -1, -1, -1, -1 };       /* For the foundation card values; note that it is possible for card values to be 0 */
    FILE *file = fopen(SOLIT_SAVE, "w");
    if (file == NULL) {
        printf("Error opening save file in write mode: returning\n");
        return;
    }
    /* Write out the number of moves, and the waste index; tbl_first[i] go on the next line. */
    fprintf(file, "%d %d\n", nmoves, waste_index);
    fprintf(file, "%d", tbl_first[0]);
    for (int i = 1; i < 7; i++)
        fprintf(file, " %d", tbl_first[i]);
    fprintf(file, "\n");
    
    /* Record the g_fdtion_top cards as single card values; then save the stock/tableau hands */
    for (int i = 0; i < 4; i++)
        if (g_fdtion_top[i]->number > 0)
            vals[i] = get_card_value(g_fdtion_top[i]);
    fprintf(file, "%d %d %d %d\n", vals[0], vals[1], vals[2], vals[3]);

    fsave_hand(g_stock_hand, file);
    for (int i = 0; i < 7; i++)
        fsave_linked_hand(g_tbl_hand[i], file);
        
    if (fsave_stats() < 0) {
        printf("Error saving player stats: returning\n");
        return;
    }
    fclose(file);
    printf("\nSuccessfully saved game!\n\n");
}

void start_new_solitgame()
{
    printf("Starting new game...\n");
    game_destroy();
    
    int count = 0;                                  /* For counting the index of the generated deck */
    struct Hand *deck = gen_ordered_deck();         /* Temporary - will free right after using */
    shuffle_hand(deck, SHUFFLE_AMOUNT);
    
    /* Allocate hands/foundation cards, then fill up tableau hands */
    game_init();

    for (count = 0; count < NCARDS_STOCK; count++)  /* Note that Hand copies the cards from the deck; the LinkedHands do not */
        g_stock_hand->cards[count] = deck->cards[count];
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < i + 1; j++)
            linked_hand_add(g_tbl_hand[i], card_node_create(&deck->cards[count++], NULL));

        tbl_first[i] = i;
    }
    free_hand(deck, 1);
    
    nmoves = 0;
    waste_index = -1;
    g_solit_curr_state = START;
    play_game();
}

void start_saved_solitgame()
{
    game_destroy();

    int moves = 0, vals[4] = { -1, -1, -1, -1 };
    struct Hand **temp;                     /* For storing fopen_hand() arrays (of pointers) */
    FILE *file = fopen(SOLIT_SAVE, "r");
    if (file == NULL) {
        printf("\nNo save file found, or error opening file.\n");
        show_solit_menu();
        return;
    }

    /* Get the move number, waste index, tbl_first[i] */
    game_init();
    fscanf(file, "%9d", &nmoves);
    fscanf(file, "%2d", &waste_index);
    while (fgetc(file) != '\n')
        ;
    for (int i = 0; i < 7; i++)
        fscanf(file, "%2d", &tbl_first[i]);
    while (fgetc(file) != '\n')
        ;

    /* Foundation piles */
    fscanf(file, "%2d %2d %2d %2d", &vals[0], &vals[1], &vals[2], &vals[3]);
    while (fgetc(file) != '\n')
        ;
    for (int i = 0; i < 4; i++) {
        if (vals[i] >= 0) {                 /* Initialize them when appropriate; else leave the -1 values */
            g_fdtion_top[i]->number = vals[i] % 13 + 1;
            g_fdtion_top[i]->suit = vals[i] / 13;
        }
    }

    /* Open the stock hand and the LinkedHands */
    free_hand(g_stock_hand, 1);
    temp = fopen_hand(file, 1);
    g_stock_hand = temp[0];
    free(temp);

    temp = fopen_hand(file, 7);
    for (int i = 0; i < 7; i++) {
        fill_linked_hand(g_tbl_hand[i], temp[i]);
        free_hand(temp[i], 1);
    }
    free(temp);

    free(player->hand);
    fclose(file);

    nmoves = moves;
    g_solit_curr_state = START;
    play_game();
}

void solitaire()
{
    while (1) {                             /* Infinite loop until quit_solitgame() is called */
        PTFV fptr = show_solit_menu();
        (*fptr)();
    }
}

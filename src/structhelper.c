#include "structhelper.h"
#include "carddefs.h"
#include "gamedefs.h"

#include <stdlib.h>
#include <string.h>

struct CardNode * card_node_create(struct Card *card, struct CardNode *next)
{
    struct CardNode *node = malloc(sizeof(struct CardNode));
    node->card = malloc(sizeof(struct Card));
    *node->card = *card;                    /* Set card in new node; DOES allocate new memory for the card */
    node->next = next;

    return node;
}

void free_hand(struct Hand *hand, int do_free_hand)
{
    free(hand->isplayed);
    free(hand->cards);
    if (do_free_hand)
        free(hand);
}

void free_linked_hand(struct LinkedHand *hand, int do_free_cards)
{
    struct CardNode *node, *prev;

    node = hand->node;
    while (node != NULL) {                  /* Go through nodes, freeing current one and going on to the next */
        if (do_free_cards)
            free(node->card);
        prev = node;
        node = node->next;
        free(prev);
    }
    free(hand);
}

struct Hand * hand_create(int ncards)
{
    struct Hand *hand = malloc(sizeof(struct Hand));
    hand->ncards = ncards;
    hand->cards = malloc(ncards * sizeof(struct Card));
    hand->isplayed = malloc(ncards * sizeof(int));
    memset(hand->isplayed, 0, ncards * sizeof(int));

    return hand;
}

struct LinkedHand * linked_hand_create(void)
{
    struct LinkedHand *hand = malloc(sizeof(struct LinkedHand));
    hand->ncards = 0;
    hand->node = NULL;                      /* Don't malloc() a CardNode unless you're creating a valid CardNode */

    return hand;
}

void playerl_init(struct Player_L *player, int wins, int losses, int curr_score)
{
    player->nwins = wins;
    player->nlosses = losses;
    player->curr_score = curr_score;
}

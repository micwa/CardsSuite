#include "solitlogic.h"
#include "solitgame.h"
#include "solitdefs.h"
#include "carddefs.h"
#include "cardutility.h"
#include "structhelper.h"

#include <stdlib.h>

int is_valid_move(struct SolitMove *move)
{
    struct Card *src, *dest;

    /* Determine src and dest for comparison */
    if (move->type == WASTE_TO_FDTION || move->type == TBL_SINGLE_TO_FDTION) {
        src = (struct Card *)move->src;
        dest = (struct Card *)move->dest;
    } else if (move->type == WASTE_TO_TBL_EMPTY || move->type == WASTE_TO_TBL) {
        src = (struct Card *)move->src;
        dest = linked_hand_get_card((struct LinkedHand *)move->dest,
                                    ((struct LinkedHand *)move->dest)->ncards - 1);
    } else if (move->type == TBL_TO_TBL_EMPTY || move->type == TBL_TO_TBL) {
        src = ((struct CardNode *)move->src)->card;
        dest = linked_hand_get_card((struct LinkedHand *)move->dest,
                                    ((struct LinkedHand *)move->dest)->ncards - 1);
    } /* Don't do anything if otherwise */

    switch (move->type) {                         /* Early cases that disregard src and/or dest */
        case FLIP_STOCK:
            return 1;
        case NONE:
        case TBL_TO_FDTION:
            return -1;
        case WASTE_TO_TBL_EMPTY:
        case TBL_TO_TBL_EMPTY:
            return (src->number == 13) ? 1 : -1;
    }

    int opp_suits, num_src = src->number, num_dest = dest->number;
    enum Suit suit_src = src->suit, suit_dest = dest->suit;

    /* Determine whether the suits are opposite */
    if (suit_src == CLUB || suit_src == SPADE) {
        if (suit_dest == DIAMOND || suit_dest == HEART)
            opp_suits = 1;
        else
            opp_suits = 0;
    } else {
        if (suit_dest == CLUB || suit_dest == SPADE)
            opp_suits = 1;
        else
            opp_suits = 0;
    }

    /* Now deciding based on the MoveType */
    switch (move->type) {
        case WASTE_TO_TBL:
        case TBL_TO_TBL:
            if (opp_suits && (num_dest-num_src) == 1)
                return 1;
            else
                return -1;
        case WASTE_TO_FDTION:
        case TBL_SINGLE_TO_FDTION:
            if (dest->number < 1) {         /* Don't assume that number will be -1 */
                if (num_src == 1)
                    return 1;
                else
                    return -1;
            } else {
                if (suit_src == suit_dest && (num_src-num_dest) == 1)
                    return 1;
                else
                    return -1;
            }
        default:
            return 0;
    }
}

int make_move(struct SolitMove *move)
{
    struct Card *card;
    struct CardNode *node;

    switch (move->type) {
        case FLIP_STOCK:                    /* Do nothing, in case caller did not check is_valid_move() (which they should) */
        case NONE:
        case TBL_TO_FDTION:
            return -1;
        case WASTE_TO_TBL_EMPTY:            /* src = Card, dest = LinkedHand */
        case WASTE_TO_TBL:
            node = card_node_create((struct Card *)move->src, NULL);
            linked_hand_add((struct LinkedHand *)move->dest, node);
            return 1;
        case TBL_TO_TBL_EMPTY:              /* src = CardNode, dest = LinkedHand */
        case TBL_TO_TBL:                    /* Note: does NOT take care of removing node from pile */
            linked_hand_add((struct LinkedHand *)move->dest, (struct CardNode *)move->src);
            return 1;
        case WASTE_TO_FDTION:               /* src = Card, dest = Card */
        case TBL_SINGLE_TO_FDTION:
            card = (struct Card *)move->dest;
            *card = *(struct Card *)move->src;      /* Copy the CARD, not the address */
            return 1;
        default:
            return 0;
    }
}

int solit_game_win(const struct Card *fdtion[4])
{
    for (int i = 0; i < 4; i++)
        if (fdtion[i] != NULL) {            /* Shouldn't be NULL in the first place */
            if (fdtion[i]->number != 13)
                return 0;
        } else
            return 0;

    return 1;
}

void undo_move(struct SolitMove *move, int *waste_index, int tbl_first[7])
{
    struct Card *card1, *card2;
    struct LinkedHand *lhand;
    struct CardNode *node;
    /* num1 stores up to 99; num2 up to 9; num3 up to whatever you want */
    int num1 = move->num % 100, num2 = move->num % 1000 / 100 - 1,
        num3 = move->num / 1000 - 1;

    switch (move->type) {
        case FLIP_STOCK:
            *waste_index = num1;
            if (*waste_index != -1)
                g_stock_hand->isplayed[*waste_index] = 0;
            break;
        case WASTE_TO_TBL_EMPTY:            /* num1 = waste_index, num2 = col_dest */
            tbl_first[num2] = -1;
            /* Fall through */
        case WASTE_TO_TBL:
            *waste_index = num1;
            g_stock_hand->isplayed[*waste_index] = 0;
            lhand = (struct LinkedHand *)move->dest;
            node = linked_hand_remove(lhand, lhand->ncards - 1, 1);
            free(node->card);
            free(node);
            break;
        case TBL_TO_TBL_EMPTY:              /* num2 = col_src, num3 = old tbl_first[num2] */
            tbl_first[num2] = -1;
            /* Fall through */
        case TBL_TO_TBL:
            lhand = (struct LinkedHand *)move->dest;
            node = linked_hand_remove(lhand, lhand->ncards - move->num % 100, 1);
            linked_hand_add(g_tbl_hand[num3], node);
            tbl_first[num2] = num3;
            break;
        case WASTE_TO_FDTION:               /* num1 = waste_index */
            *waste_index = num1;
            g_stock_hand->isplayed[*waste_index] = 0;
            card1 = (struct Card *)move->dest;
            card1->number--;
            break;
        case TBL_SINGLE_TO_FDTION:          /* num2 = col_src, num3 = old tbl_first[num2] */
            card1 = (struct Card *)move->dest;
            card1->number--;
            card2 = malloc(sizeof(struct Card));    /* Can't access move->src since already free()'d */
            card2->number = card1->number + 1;
            card2->suit = card1->suit;
            linked_hand_add(g_tbl_hand[num3], card_node_create(card2, NULL));
            tbl_first[num2] = num3;
            break;
        case NONE:                          /* Do nothing */
        case TBL_TO_FDTION:
        default:
            return;
    }
}

#include "solitlogic.h"
#include "solitdefs.h"
#include "carddefs.h"
#include "cardutility.h"
#include "structhelper.h"

#include <stdlib.h>

int is_valid_move(enum MoveType type, const struct Card *src, const struct Card *dest)
{
	switch (type) {							/* Early cases that disregard src and/or dest */
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
	switch (type) {
		case WASTE_TO_TBL:
		case TBL_TO_TBL:
			if (opp_suits && (num_dest-num_src) == 1)
				return 1;
			else
				return -1;
		case WASTE_TO_FDTION:
		case TBL_SINGLE_TO_FDTION:
			if (dest->number < 1) {			/* Don't assume that number will be -1 */
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
		case FLIP_STOCK:					/* Do nothing, in case caller did not check is_valid_move() (which they should) */
		case NONE:
		case TBL_TO_FDTION:
			return -1;
		case WASTE_TO_TBL_EMPTY:			/* src = Card, dest = LinkedHand */
		case WASTE_TO_TBL:
			node = card_node_create((struct Card *)move->src, NULL);
			linked_hand_add((struct LinkedHand *)move->dest, node);
			return 1;
		case TBL_TO_TBL_EMPTY:				/* src = CardNode, dest = LinkedHand */
		case TBL_TO_TBL:					/* Note: does NOT take care of removing node from pile */
			linked_hand_add((struct LinkedHand *)move->dest, (struct CardNode *)move->src);
			return 1;
		case WASTE_TO_FDTION:				/* src = Card, dest = Card */
		case TBL_SINGLE_TO_FDTION:
			card = (struct Card *)move->dest;
			*card = *(struct Card *)move->src;	    /* Copy the CARD, not the address */
			return 1;
		default:
			return 0;
	}
}

int solit_game_win(struct Card *fdtion[4])
{
	for (int i = 0; i < 4; i++)
		if (fdtion[i] != NULL) {			/* Shouldn't be NULL in the first place */
			if (fdtion[i]->number != 13)
				return 0;
		} else
			return 0;

	return 1;
}

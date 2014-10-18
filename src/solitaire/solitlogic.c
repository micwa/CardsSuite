#include "solitlogic.h"
#include "carddefs.h"

int is_valid_move(enum MoveType type, const struct Card *src, const struct Card *dest)
{
	if (type == FLIP_STOCK || type == NONE)	/* Automatically return true/false for obvious cases */
		return 1;
	if (type == TBL_TO_FDTION)
		return -1;

	switch (type) {							/* Early cases that may have dest == NULL */
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
	switch (type)
	{
		case WASTE_TO_TBL:
		case TBL_TO_TBL:
			if (opp_suits && (num_dest-num_src) == 1)
				return 1;
			else
				return -1;
		case WASTE_TO_FDTION:
		case TBL_SINGLE_TO_FDTION:
			if (suit_src == suit_dest && (num_dest-num_src) == 1)
				return 1;
			else
				return -1;
		default:
			return 0;
	}
}

void make_move(enum MoveType type, void *src, void *dest);

int solit_game_win()
{
	extern struct Card *g_fdtion_top[4];
	for (int i = 0; i < 4; i++)
		if (g_fdtion_top[i]->number != 13)
			return -1;

	return 1;
}

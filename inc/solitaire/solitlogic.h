#ifndef SOLITLOGIC_H_
#define SOLITLOGIC_H_

#include "carddefs.h"

enum MoveType { WASTE_TO_TBL,
    WASTE_TO_TBL_EMPTY,                     /* From waste to an empty tableau pile */
    WASTE_TO_FDTION, 
    TBL_TO_TBL,                             /* TBL = any amount of tableau cards, TO_TBL = non-empty pile */
    TBL_TO_TBL_EMPTY,
    TBL_SINGLE_TO_FDTION,
    TBL_TO_FDTION,					        /* Always invalid; next two are always true */
    FLIP_STOCK,                             /* No FLIP_TBL since solitgame will automatically flip */
    NONE
};

/* Returns 1 is the moving card src to dest is a valid move, and -1 if not.
 * For FLIP_STOCK and FLIP_TBL, this function will immediately return 1.
 * Returns 0 on if type if not a valid MoveType. */
int is_valid_move(enum MoveType type, const struct Card *src, const struct Card *dest);

/* Makes the specified move from src to dest using solitgame.c's stock_hand,
 * tbl_hand, and fdtion_hand. Determines the correct type by looking at the MoveType. */
void make_move(enum MoveType type, void *src, void *dest);

/* Returns 1 if the game is won or not (i.e. all foundation piles are filled, -1 if not */
int solit_game_win();

#endif /* SOLITLLOGIC_H_ */

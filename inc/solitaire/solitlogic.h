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
    FLIP_STOCK,
    NONE
};

/* Returns 1 is the moving card src to dest is a valid move, and -1 if not.
 * For FLIP_STOCK and FLIP_TBL, this function will immediately return 1.
 * Returns 0 if type is not a valid MoveType. */
int is_valid_move(enum MoveType type, const struct Card *src, const struct Card *dest);

/* Makes the specified move from src to dest using solitgame.c's stock_hand,
 * tbl_hand, and fdtion_hand. Makes the move regardless of whether it is a valid
 * solitaire move, and does ONLY that (i.e. does not remove nodes).
 * This determines the correct type by looking at the MoveType.
 * Returns 1 if the move is successful, -1 if not, and 0 if invalid argument(s). */
int make_move(enum MoveType type, void *src, void *dest);

/* Returns 1 if the game is won (i.e. all foundation piles are filled), 0 if not */
int solit_game_win(struct Card *fdtion[4]);

#endif /* SOLITLLOGIC_H_ */

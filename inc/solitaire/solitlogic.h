#ifndef SOLITLOGIC_H_
#define SOLITLOGIC_H_

#include "solitdefs.h"
#include "carddefs.h"

/* Returns 1 is the moving card src to dest is a valid move, and -1 if not.
 * For FLIP_STOCK and FLIP_TBL, this function will immediately return 1.
 * Returns 0 if type is not a valid MoveType. */
int is_valid_move(enum MoveType type, const struct Card *src, const struct Card *dest);

/* Makes the specified move represented by the given move, and executes it regardless
 * of whether it is a valid solitaire move, and does ONLY that (e.g. does not remove nodes).
 * This determines the correct type by looking at the MoveType.
 * Returns 1 if the move is successful, -1 if not, and 0 if error. */
int make_move(struct SolitMove *move);

/* Returns 1 if the game is won (i.e. all foundation piles are filled), 0 if not */
int solit_game_win(const struct Card *fdtion[4]);

/* Undoes the move represented by the given move (the opposite of make_move(), basically).
 * Does not remove any nodes or change any indices.
 * Returns a void pointer to whatever it removes (if anything) and NULL otherwise. */
void * undo_move(struct SolitMove *move);

#endif /* SOLITLLOGIC_H_ */

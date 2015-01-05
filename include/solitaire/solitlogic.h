#ifndef SOLITLOGIC_H_
#define SOLITLOGIC_H_

#include "solitdefs.h"
#include "carddefs.h"

/* Returns 1 is the SolitMove is a valid move (from src to dest), and -1 if not.
 * For FLIP_STOCK and FLIP_TBL, this function will immediately return 1.
 * Returns 0 if type is not a valid MoveType. */
int is_valid_move(struct SolitMove *move);

/* Makes the specified move represented by the given move, and executes it regardless
 * of whether it is a valid solitaire move, and does ONLY that (e.g. does not remove nodes).
 * This determines the correct type by looking at the MoveType.
 * Returns 1 if the move is successful, -1 if not, and 0 if error. */
int make_move(struct SolitMove *move);

/* Returns 1 if the game is won (i.e. all foundation piles are filled), 0 if not */
int solit_game_win(const struct Card *fdtion[4]);

/* Undoes the move represented by the given move (the opposite of make_move(), basically)
 * and updates the given waste_index and tbl_firsts as appropriate. */
void undo_move(struct SolitMove *move, int *waste_index, int tbl_first[7]);

#endif /* SOLITLLOGIC_H_ */

#ifndef SOLITUI_H_
#define SOLITUI_H_

typedef void (*PTFV)();                     /* A pointer to a function taking zero arguments and returning void */

/* Draws the stock/waste, the foundation, and the tableau cards; alphabet characters
 * represent rows, numbers = columns, "s" = flipping stock, "t" = move card from waste,
 * and "w", "x", "y", "z" for moving to the foundation (also "u" for undo).
 * waste_index is the current index of the card in stock_hand that will be drawn (-1 for no card).
 * If draws_rows < 1, the row alphabet is not drawn.
 * If draws_cols < 1, the column numbers is not drawn.
 */
void draw_solit_board(int waste_index, int tbl_first[7], int draw_rows, int draw_cols);

/* Displays the menu (different depending on current gamestate) */
PTFV show_solit_menu(enum GameState curr_state);

#endif /* SOLITUI_H_ */

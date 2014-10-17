#ifndef SOLITUI_H_
#define SOLITUI_H_

/* Draws the stock/waste, the foundation, and the tableau cards; alphabet characters
 * represent rows, numbers = columns, "s" = flipping stock, "t" = move card from waste,
 * and "w", "x", "y", "z" for moving to the foundation (also "u" for undo).
 * waste_index is the current index of the card in stock_hand that will be drawn (-1 for no card).
 * If draws_rows < 1, the row alphabet is not drawn.
 * If draws_cols < 1, the column numbers is not drawn.
 */
void draw_solit_board(int waste_index, int tbl_first[7], int draw_rows, int draw_cols);

/* Frees card_encs if they were init'd already */
void free_card_encs();

/* Retrieves all 52 card encodings and stores them in an array. This MUST be called
 * for draw_solit_board() to work. */
void init_card_encs();

/* Displays the menu (different depending on current gamestate) */
void show_solit_menu();

#endif /* SOLITUI_H_ */

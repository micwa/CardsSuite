#ifndef SOLITUI_H_
#define SOLITUI_H_

/* Draws the stock/waste, the foundation, and the tableau cards; alphabet characters
 * represent rows, numbers = columns, "s" = flipping stock, "t" = move card from waste,
 * and "w", "x", "y", "z" for moving to the foundation (also "u" for undo).
 * waste_index is the current index of the card in stock_hand that will be drawn (-1 for no card).
 * If only_rows > 0, only the row alphabet is drawn.
 * If only_cols > 0, only the column numbers are drawn.
 */
void draw_solit_board(int *waste_index, int only_rows, int only_cols);

/* Displays the menu (different depending on current gamestate) */
void show_solit_menu();

#endif /* SOLITUI_H_ */

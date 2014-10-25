#ifndef WARUI_H_
#define WARUI_H_

#include "carddefs.h"
#include "gamedefs.h"

typedef void (*PTFV)();						/* A pointer to a function taking zero arguments and returning void */

/* Draws the two hands on the console screen */
void draw_war_board(const struct Player_L *player, const struct Player_L *cpu, const struct Card *cards[2]);

/* Prints game statistics, and should return after one character is inputted */
void print_war_stats();

/* Displays the menu (different depending on current gamestate) and returns a pointer to the
 * function (start_new_game(), print_war_stats(), etc.) representing the action selected. */
PTFV show_war_menu();

#endif /* WARUI_H_ */

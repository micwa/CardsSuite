#ifndef WARUI_H_
#define WARUI_H_

#include "carddefs.h"
#include "gamedefs.h"

/* Draws the two hands on the console screen */
void draw_war_board(const struct Player *player, const struct Player *cpu);

/* Displays the menu (different depending on current gamestate) */
void show_war_menu();

#endif /* WARUI_H_ */

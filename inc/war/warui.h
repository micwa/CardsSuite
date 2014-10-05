#ifndef WARUI_H_
#define WARUI_H_

/* Prints game statistics */
static void print_stats();

/* Displays the menu (different depending on current gamestate */
void show_war_menu();

/* Various menus */
static void show_menu_start();
static void show_menu_pause();
static void show_menu_win();
static void show_menu_lose();

#endif /* WARUI_H_ */

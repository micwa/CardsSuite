#ifndef WARGAME_H_
#define WARGAME_H_

enum GameState { START, PAUSE, WIN, LOSE };

/* Displays the menu (different depending on current gamestate */
void show_war_menu();

/* Different menus */
static void show_menu_start();
static void show_menu_paused();
static void show_menu_won();
static void show_menu_lost();

/* Saves the current game state to a text file */
static void save_game();

/* Starts a new game */
static void start_new_game();

/* Starts a saved game */
static void start_saved_game();

#endif /* WARGAME_H_ */

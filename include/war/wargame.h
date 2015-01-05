#ifndef WARGAME_H_
#define WARGAME_H_

#include "gamedefs.h"

/* Declare global constants and variables */
extern const char *WAR_STATS;
extern const char *WAR_SAVE;
extern enum GameState g_war_curr_state;       /* Current game state */

/* Quits the game; also should call game_destroy() */
void quit_wargame();

/* Resumes the current game */
void resume_wargame();

/* Saves the current game state to a text file */
void save_wargame();

/* Starts a new game */
void start_new_wargame();

/* Starts a saved game */
void start_saved_wargame();

/* Lanches war, i.e. this game.
 * So that the call stack doesn't grow huge, war() is the "base" function
 * and should be the only one that calls show_war_menu(). It should loop to show
 * the war menu any time a (called) function exits, so make sure the curr_state is
 * set properly after the execution of each function. */
void war();

#endif /* WARGAME_H_ */

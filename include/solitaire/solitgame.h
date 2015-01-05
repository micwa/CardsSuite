#ifndef SOLITGAME_H_
#define SOLITGAME_H_

#include "gamedefs.h"

/* Declare global constants and variables */
extern const char *SOLIT_STATS;
extern const char *SOLIT_SAVE;

extern enum GameState g_solit_curr_state;
extern struct Hand *g_stock_hand;
extern struct LinkedHand *g_tbl_hand[7];
extern struct Card *g_fdtion_top[4];

/* Quits the game; also should call game_destroy() */
void quit_solitgame();

/* Resumes the current game */
void resume_solitgame();

/* Saves the current game state to a text file */
void save_solitgame();

/* Starts a new game */
void start_new_solitgame();

/* Starts a saved game */
void start_saved_solitgame();

/* Launches solitaire, i.e. this game */
void solitaire();

#endif /* SOLITGAME_H_ */

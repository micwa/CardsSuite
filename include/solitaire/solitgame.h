#ifndef SOLITGAME_H_
#define SOLITGAME_H_

#include "gamedefs.h"

/* Declare global constants and variables */
extern const char *SOLIT_STATS;
extern const char *SOLIT_SAVE;

extern struct Hand *g_stock_hand;
extern struct LinkedHand *g_tbl_hand[7];
extern struct Card *g_fdtion_top[4];

/* Quits the game; also should call game_destroy() */
void quit_solitgame(void);

/* Resumes the current game */
void resume_solitgame(void);

/* Saves the current game state to a text file */
void save_solitgame(void);

/* Starts a new game */
void start_new_solitgame(void);

/* Starts a saved game */
void start_saved_solitgame(void);

/* Launches solitaire, i.e. this game */
void solitaire(void);

#endif /* SOLITGAME_H_ */

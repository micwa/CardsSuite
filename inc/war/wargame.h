#ifndef WARGAME_H_
#define WARGAME_H_

/* Declare global constants and variables */
extern const char *WAR_STATS;
extern const char *WAR_SAVE;
extern enum GameState war_curr_state;       /* Current game state */

/* Quits the game; also should call players_destroy() */
void quit_wargame();

/* Resumes the current game */
void resume_wargame();

/* Saves the current game state to a text file */
void save_wargame();

/* Starts a new game */
void start_new_wargame();

/* Starts a saved game */
void start_saved_wargame();

/* Launches war, i.e. this game */
void war();

#endif /* WARGAME_H_ */

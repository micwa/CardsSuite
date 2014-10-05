#ifndef WARGAME_H_
#define WARGAME_H_

enum GameState { START, PAUSE, WIN, LOSE };

/* Quits the game and frees all malloc'd memory */
void quit_wargame();

/* Saves the current game state to a text file */
void save_wargame();

/* Starts a new game */
void start_new_wargame();

/* Starts a saved game */
void start_saved_wargame();

/* Launches war, i.e. this game */
void war();

#endif /* WARGAME_H_ */

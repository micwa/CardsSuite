#ifndef SOLITGAME_H_
#define SOLITGAME_H_

/* Quits the game; also should call hands_destroy() */
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

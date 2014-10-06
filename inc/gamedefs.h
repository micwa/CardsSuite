#ifndef GAMEDEFS_H_
#define GAMEDEFS_H_

enum GameState { START, PAUSE, WIN, LOSE };

struct Player {
    struct Hand *hand;
    int curr_score;
    int nwins, nlosses;
};

#endif /* GAMEDEFS_H_ */

#ifndef GAMEDEFS_H_
#define GAMEDEFS_H_

#include "carddefs.h"

enum GameState { START, PAUSE, WIN, LOSE, QUIT };

struct Player {
    struct Hand *hand;
    int curr_score;
    int nwins, nlosses;
};

struct Player_L {
    struct LinkedHand *hand;
    int curr_score;
    int nwins, nlosses;
};

#endif /* GAMEDEFS_H_ */

#ifndef CARDDEFS_H_
#define CARDDEFS_H_

enum Suit { CLUB, DIAMOND, SPADE, HEART };

/* A card with a number and a suit. */
struct Card {
    int number;
    enum Suit suit;
};

/* An array of cards, basically. */
struct Hand {
    int ncards;             /* Should equal the number of cards in the array */
    struct Card *cards;
};

#endif /* CARDDEFS_H_ */

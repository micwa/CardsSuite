#ifndef CARDDEFS_H_
#define CARDDEFS_H_

/* Prefix and suffixes for the suits; latter should be in the order they are enumerated */
extern const char *CARD_UTF_PREFIX;
extern const char *CARD_SUFFIXES[4];
extern const char *CARD_BACK;

extern char *card_encs[52];

enum Suit { CLUB, DIAMOND, HEART, SPADE };

/* A card with a number and a suit. */
struct Card {
    int number;
    enum Suit suit;
};

/* An array of cards with an array of ints to keep track of which cards have been played. */
struct Hand {
    int ncards;             /* Should equal the number of cards in the array */
    struct Card *cards;
    int *isplayed;          /* Should be size equal to ncards */
};

/* Wraps a Card in a linked list node */
struct CardNode {
    struct CardNode *next;
    struct Card *card;
};

/* A linked list, with ncards keeping track of the number of cards */
struct LinkedHand {
    int ncards;
    struct CardNode *node;
};

#endif /* CARDDEFS_H_ */

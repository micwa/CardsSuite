#ifndef CARDDEFS_H_
#define CARDDEFS_H_

/* Prefix and suffixes for the suits; latter should be in the order they are enumerated */
extern const char *CARD_UTF_PREFIX;
extern const char *CARD_SUFFIXES[4];

enum Suit { CLUB, DIAMOND, HEART, SPADE };

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

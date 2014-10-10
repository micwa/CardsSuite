#ifndef CARDUTIL_H_
#define CARDUTIL_H_

#include "carddefs.h"

/* Returns the number of cards that have been played in the given hand  */
int cards_played(const struct Hand *hand);

/* Returns 0 if the cards are the same; <0 if c1 is lower than c2; and >0 otherwise.
 * The ranking of suits is defined in carddefs.h */
int card_compare(const struct Card *c1, const struct Card *c2);

/* Fills the given LinkedHand (assumes its CardNode has been initialized already, and
overwrites any cards that one currently contains) with the given Hand. */
void fill_linked_hand(struct LinkedHand *l_hand, const struct Hand *hand);

/* Creates an array of hands from the hand(s) stored in filename; the user must specify
 * the number of hands (i.e. number of lines to read). 
 * Distinguishes beween regular Hands and LinkedHands - for LinkedHands, set isplayed
 * to all 0s. (implement later) */
struct Hand * fopen_hand(const char *filename, int nhands);

/* Free the memory allocated for the Hand AND its members (cards, isplayed) */
void free_hand(struct Hand *hand);

/* Frees the memory allocated for the LinkedHand AND for all CardNodes (and for cards if specified) */
void free_linked_hand(struct LinkedHand *hand, int do_freecards);

/* Save the given LinkedHand to the specified file, writing using the specified fopen mode.
 * This function should write each hand on one line, and append a newline character at the end. */
int fsave_linked_hand(const struct LinkedHand *hand, const char *filename, const char *mode);

/* Returns a Hand with 52 cards in order from Ace to King, in the suit order specified
 * by carddefs.h */
struct Hand gen_ordered_deck();

/* Returns a random card */
struct Card gen_random_card();

/* Returns the encoding in UTF-8 of the playing card */
char * get_card_encoding(const struct Card *card);

/* Returns an integer from 0 to 51 such that each card is mapped to a unique integer */
int get_card_value(const struct Card *card);

/* Returns the name of the card, where 11 = jack, 12 = queen, etc.
 * in the format: [Number], [Suit]; e.g. King of spades would be King, Spade */
char * get_card_name(const struct Card *card);

/* Adds the card to the linked list by placing the given CardNode at the end */
void linked_hand_add(struct LinkedHand *hand, struct CardNode *node);

/* Shuffles the given hand ntimes times */
void shuffle_hand(struct Hand *hand, int ntimes);

/* Splits the hand (or deck) into the specified number of Hands (i.e. an array) */
struct Hand * split_hand(struct Hand *hand, int nhands);

#endif /* CARDUTIL_H_ */

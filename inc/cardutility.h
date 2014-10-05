#ifndef CARDUTIL_H_
#define CARDUTIL_H_

#include "carddefs.h"

/* Returns 0 if the cards are the same; <0 if c1 is lower than c2; and >0 otherwise.
 * The ranking of suits is defined in carddefs.h */
int card_compare(const struct Card c1, const struct Card c2);

/* Returns a random card */
struct Card gen_random_card();

/* Returns a Hand with 52 cards, all shuffled */
struct Hand gen_random_deck();

/* Returns the encoding in UTF-8 of the playing card */
char * get_card_encoding(const struct Card card);

/* Returns an integer from 0 to 51 such that each card is mapped to a unique integer */
int get_card_value(const struct Card card);

/* Returns the name of the card, where 11 = jack, 12 = queen, etc.
 * in the format: [Number], [Suit]; e.g. King of spades would be King, Spade */
char * get_card_name(const struct Card card);

/* Splits the hand (or deck) into the specified number of Hands */
struct Hand * split_hand(struct Hand hand, int nhands);

#endif /* CARDUTIL_H_ */

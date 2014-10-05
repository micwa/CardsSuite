#ifndef CARDUTIL_H_
#define CARDUTIL_H_

#include "carddefs.h"

extern const char *CARD_UTF8[][13];

struct Card gen_random_card();      /* Returns a random card */
struct Hand gen_random_deck();      /* Returns a Hand with 52 cards, all shuffled */
int get_card_value(struct Card card);        /* Returns an integer from 0 to 51 such that each card is mapped to a unique integer */
char * get_card_name(struct Card card);      /* Returns the name of the card, where 11 = jack, 12 = queen, etc.
                                        in the format: [Number], [Suit]; e.g. King of spades would be King, Spade */

#endif /* CARDUTIL_H_ */

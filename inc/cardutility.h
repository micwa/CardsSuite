#ifndef CARDUTIL_H_
#define CARDUTIL_H_

#include "carddefs.h"
#include <stdio.h>

/* Stores all 52 card encodings for convenience; call init_card_encs() to initialize,
 * and free_card_encs() to release the memory */
extern char *g_card_encs[52];

/* Returns the number of cards that have been played in the given hand  */
int cards_played(const struct Hand *hand);

/* Returns 0 if the cards are the same; <0 if c1 is lower than c2; and >0 otherwise.
 * The ranking of suits is defined in carddefs.h */
int card_compare(const struct Card *c1, const struct Card *c2);

/* Fills the given LinkedHand with the given Hand. If CardNode is NULL, initializes it; otherwise,
this sets the CardNode's card to the Hand's first card (note: if the card exists, this will overwrite
it and might cause a memory leak). */
void fill_linked_hand(struct LinkedHand *l_hand, const struct Hand *hand);

/* Creates an array of hands from the hand(s) stored in the FILE; the user must specify
 * the number of hands (i.e. number of lines to read). 
 * Distinguishes between regular Hands and LinkedHands - for LinkedHands, set isplayed
 * to all 0s. (implement later) */
struct Hand * fopen_hand(FILE *file, int nhands);

/* Frees all_card_encs if they were init'd already */
void free_card_encs();

/* Save the given LinkedHand to the specified file. This function should write the whole
 * hand on one line, and append a newline character at the end. */
void fsave_linked_hand(const struct LinkedHand *hand, FILE *file);

/* Returns a Hand pointer with 52 cards in order from Ace to King, in the suit order
 * specified by carddefs.h */
struct Hand * gen_ordered_deck();

/* Returns a random card */
struct Card gen_random_card();

/* Returns the encoding in UTF-8 of the playing card */
char * get_card_encoding(const struct Card *card);

/* Returns an integer from 0 to 51 such that each card is mapped to a unique integer */
int get_card_value(const struct Card *card);

/* Returns the name of the card, where 11 = jack, 12 = queen, etc.
 * in the format: [Number], [Suit]; e.g. King of spades would be King, Spade */
char * get_card_name(const struct Card *card);

/* Retrieves all 52 card encodings and stores them in an array. This MUST be called
 * for if a game accesses card_encs for its card strings. */
void init_card_encs();

/* Adds the card to the linked list by placing the given CardNode at the end.
 * If the LinkedHand's CardNode is null, makes that node the given node. */
void linked_hand_add(struct LinkedHand *hand, struct CardNode *node);

/* Returns the indexth card (starting at 0) in the LinkedHand */
struct Card * linked_hand_get_card(struct LinkedHand *hand, int index);

/* Shuffles the given hand ntimes times */
void shuffle_hand(struct Hand *hand, int ntimes);

/* Splits the hand (or deck) into the specified number of Hands (i.e. an array) */
struct Hand * split_hand(struct Hand *hand, int nhands);

#endif /* CARDUTIL_H_ */

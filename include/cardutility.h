#ifndef CARDUTIL_H_
#define CARDUTIL_H_

#include "carddefs.h"

/* Stores all 52 card encodings for convenience; call init_card_encs() to initialize,
 * and free_card_encs() to free the memory. */
extern char *g_card_encs[52];

/* Returns the number of cards that have been played in the given hand  */
int cards_played(const struct Hand *hand);

/* Returns 0 if the cards are the same; <0 if c1 is lower than c2; and >0 otherwise.
 * The ranking of suits is defined in carddefs.h */
int card_compare(const struct Card *c1, const struct Card *c2);

/* Fills the given LinkedHand with the given Hand. This function assumes that the given
 * LinkedHand is empty, i.e. creates a new node for the LinkedHand regardless of whether
 * one already exists or not. */
void fill_linked_hand(struct LinkedHand *l_hand, const struct Hand *hand);

/* Frees all_card_encs if init_card_encs() had been called; otherwise, does nothing. */
void free_card_encs(void);

/* Returns a Hand pointer with 52 cards in order from Ace to King, in the suit order
 * specified by carddefs.h */
struct Hand * gen_ordered_deck(void);

/* Returns a random card */
struct Card gen_random_card(void);

/* Returns the encoding in UTF-8 of the playing card */
char * get_card_encoding(const struct Card *card);

/* Returns an integer from 0 to 51 such that each card is mapped to a unique integer */
int get_card_value(const struct Card *card);

/* Returns the name of the card, where 11 = jack, 12 = queen, etc.
 * in the format: [Number], [Suit]; e.g. King of spades would be King, Spade */
char * get_card_name(const struct Card *card);

/* Returns the index of the next unplayed card starting at the specified index (includes that index).
 * If do_loop is non-zero, will loop around to 0 to index - 1 if no unplayed cards are found.
 * If there are no unplayed cards, returns -1. */
int get_next_unplayed(struct Hand *hand, int index, int do_loop);

/* Retrieves all 52 card encodings and stores them in an array (if they have not yet been
 * initialized). This MUST be called if you require access to g_card_encs for its card strings. */
void init_card_encs(void);

/* Adds the given CardNode at the end of the LinkedHand, and increments ncards
 * by 1. If the LinkedHand's CardNode is null, makes that node the given node. */
void linked_hand_add(struct LinkedHand *hand, struct CardNode *node);

/* Returns the indexth card (starting at 0) in the LinkedHand */
struct Card * linked_hand_get_card(struct LinkedHand *hand, int index);

/* Returns the indexth CardNode (starting at 0) in the LinkedHand */
struct CardNode * linked_hand_get_node(struct LinkedHand *hand, int index);

/* Removes the CardNode at the specified index from the LinkedHand, and decrements
 * ncards by 1. Returns the removed CardNode (unmodified).
 * If remove_all is 0, the CardNode at index - 1 (if index != 0) will have
 * its "next" pointer the CardNode at index + 1 (NULL if removing last node).
 * Otherwise, the CardNode at index - 1 will have "next" always point to NULL. */
struct CardNode * linked_hand_remove(struct LinkedHand *hand, int index, int remove_all);

/* Shuffles the given hand ntimes times */
void shuffle_hand(struct Hand *hand, int ntimes);

/* Splits the hand (or deck) into the specified number of Hands (i.e. an array of
 * Hand pointers). Must free() both the array and the returned hands (and members). */
struct Hand ** split_hand(struct Hand *hand, int nhands);

#endif /* CARDUTIL_H_ */

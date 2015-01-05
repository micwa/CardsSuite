#ifndef STRUCTHELPER_H_
#define STRUCTHELPER_H_

#include "carddefs.h"
#include "gamedefs.h"

/* Creates a CardNode containing a copy of the given card (must free the Card later)
 * and "next" set to the given node. ALWAYS use this instead of manually malloc()ing
 * a CardNode to ensure "next" is initialized to NULL, or a non-garbage value. */
struct CardNode * card_node_create(struct Card *card, struct CardNode *next);

/* Free the memory allocated for the Hand AND its members (cards, isplayed) */
void free_hand(struct Hand *hand, int do_free_hand);

/* Frees the memory allocated for the LinkedHand AND for all CardNodes (and for cards if specified) */
void free_linked_hand(struct LinkedHand *hand, int do_free_cards);

/* Creates a Hand with the specified number of cards, and isplayed set to all 0 */
struct Hand * hand_create(int ncards);

/* Creates a LinkedHand with ncards = 0, and the CardNode set NULL  */
struct LinkedHand * linked_hand_create();

/* Initializes the given Player_L with the given values. Player_L must be a valid pointer */
void playerl_init(struct Player_L *player, int wins, int losses, int curr_score);

#endif /* STRUCTHELPER_H_ */

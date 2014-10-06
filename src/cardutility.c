#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "cardutility.h"
#include "carddefs.h"

/* Define constants from carddefs.h */
const char *CARD_UTF_PREFIX = "\xf0\x9f";
const char *CARD_SUFFIXES[4] = { "\x83\x91", "\x83\x81", "\x82\xb1" , "\x82\xa1" };
const char *CARD_BACK = "\xf0\x9f\x82\xa0";

/* Private constants/variables */
static const int SHUFFLE_AMOUNT = 1000;
static int isrand_init = 0;

int all_cards_played(const struct Hand *hand)
{
    for (int i = 0; i < hand->ncards; i++)
        if (hand->isplayed[i] == 0)
            return 0;
    return 1;
}

int card_compare(const struct Card c1, const struct Card c2)
{
	if (c1.number < c2.number)
		return -1;
	else if (c1.number > c2.number)
		return 1;
	else
		return (c1.suit == c2.suit) ? 0 : c1.suit - c2.suit;
}

struct Card gen_random_card()
{
    int num, suit;
    struct Card card;

    if (!isrand_init) {
    	srand(time(NULL));
    	isrand_init = 1;
    }
    num = rand() % 13 + 1;					/* Must be in between 1 and 13 */
    suit = rand() % 4;
    card.number = num;
    card.suit = suit;

    return card;
}

struct Hand gen_random_deck()
{
    struct Hand hand;
    struct Card *cards = malloc(52 * sizeof(struct Card));
    struct Card temp_card;
    int swap1, swap2;
    int *isplayed;

    for (int i = 0; i < 52; i++) {			/* Populate the deck */
        cards[i].number = i % 13 + 1;
        cards[i].suit = i / 13;
    }

    if (!isrand_init) {
       	srand(time(NULL));
       	isrand_init = 1;
    }
    for (int i = 0; i < SHUFFLE_AMOUNT; i++) {
        swap1 = rand() % 52;
        do {
            swap2 = rand() % 52;
        } while (swap1 == swap2);

        temp_card = cards[swap1];
        cards[swap1] = cards[swap2];
        cards[swap2] = temp_card;
    }
    
    hand.ncards = 52;
    hand.cards = cards;						/* Remember to free() this... */
    isplayed = malloc(hand.ncards * sizeof(int));
    memset(isplayed, 0, hand.ncards * sizeof(int));
    hand.isplayed = &isplayed[0];                /* ...and this! */

    return hand;
}

char * get_card_encoding(const struct Card card)
{
	char *enc = malloc(4 * sizeof(char));
	strcpy(enc, CARD_UTF_PREFIX);
	strcat(enc, CARD_SUFFIXES[card.suit]);
	enc[3] += card.number - 1;				/* Some character addition */

	return enc;
}

/* Note: if there is an error, this function returns a null string. */
char * get_card_name(const struct Card card)
{
    char *name = malloc(20 * sizeof(char));
    switch (card.number) {
        case 1: strcpy(name, "1");    	break;
        case 2: strcpy(name, "2");    	break;
        case 3: strcpy(name, "3");  	break;
        case 4: strcpy(name, "4");   	break;
        case 5: strcpy(name, "5");   	break;
        case 6: strcpy(name, "6");    	break;
        case 7: strcpy(name, "7");  	break;
        case 8: strcpy(name, "8");  	break;
        case 9: strcpy(name, "9");  	break;
        case 10: strcpy(name, "10");   	break;
        case 11: strcpy(name, "Jack");  break;
        case 12: strcpy(name, "Queen"); break;
        case 13: strcpy(name, "King");  break;
        default: return '\0';
    }
    switch (card.suit) {
        case CLUB:      strcat(name, " of Clubs");     break;
        case DIAMOND:   strcat(name, " of Diamonds");  break;
        case HEART:     strcat(name, " of Hearts");    break;
        case SPADE:     strcat(name, " of Spades");    break;
        default: return '\0';
    }
    return name;
}

int get_card_value(const struct Card card)
{
    int value = 0;
    value += card.number;
    value += card.suit * 13;				/* Clubs are 0-12, Diamonds are 13-25, etc. */

    return value;
}

struct Hand * split_hand(struct Hand hand, int nhands)
{
	struct Hand *hands = malloc(nhands * sizeof(struct Hand));		/* Remember to free this! */
	int counter = 0;

	for (int i = 0; i < nhands; i++) {
		/* Assign to hand[i] an array of size 52 / nhands. Note that this
		 * uses the cards/isplayed in the Hand parameter - no new memory is allocated
         * EXCEPT for the Hands themselves. */
		hands[i].cards = &hand.cards[52 / nhands * i];
        hands[i].isplayed = &hand.isplayed[52 / nhands * i];
		hands[i].ncards = 52 / nhands;
		counter += hands[i].ncards;
	}
	hands[nhands - 1].ncards += 52 - counter;	/* Adjust for possible remainder */

	return hands;
}

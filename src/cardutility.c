#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "cardutility.h"
#include "carddefs.h"

/* Define constants from carddefs.h */
const char *CARD_UTF_PREFIX = "\xf0\x9f";
const char *CARD_SUFFIXES[4] = { "\x83\x91", "\x83\x81", "\x82\xb1" , "\x82\xa1" };

/* Private constants */
static const int SHUFFLE_AMOUNT = 1000;
static int isrand_init = 0;

int card_compare(struct Card c1, struct Card c2)
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
    hand.cards = cards;						/* Remember to free() this later! */

    return hand;
}

char * get_card_encoding(struct Card card)
{
	char *enc = malloc(4 * sizeof(char));
	strcpy(enc, CARD_UTF_PREFIX);
	strcat(enc, CARD_SUFFIXES[card.suit]);
	enc[3] += card.number - 1;				/* Some character addition */

	return enc;
}

/* Note: if there is an error, this function returns a null string. */
char * get_card_name(struct Card card)
{
    char *name = malloc(15 * sizeof(char));
    switch (card.number) {
        case 1: strcpy(name, "Ace");    break;
        case 2: strcpy(name, "Two");    break;
        case 3: strcpy(name, "Three");  break;
        case 4: strcpy(name, "Four");   break;
        case 5: strcpy(name, "Five");   break;
        case 6: strcpy(name, "Six");    break;
        case 7: strcpy(name, "Seven");  break;
        case 8: strcpy(name, "Eight");  break;
        case 9: strcpy(name, "Nine");   break;
        case 10: strcpy(name, "Ten");   break;
        case 11: strcpy(name, "Jack");  break;
        case 12: strcpy(name, "Queen"); break;
        case 13: strcpy(name, "King");  break;
        default: return '\0';
    }
    switch (card.suit) {
        case CLUB:      strcat(name, ", Club");     break;
        case DIAMOND:   strcat(name, ", Diamond");  break;
        case HEART:     strcat(name, ", Heart");    break;
        case SPADE:     strcat(name, ", Spade");    break;
        default: return '\0';
    }
    return name;
}

int get_card_value(struct Card card)
{
    int value = 0;
    value += card.number;
    value += card.suit * 13;				/* Clubs are 0-12, Diamonds are 13-25, etc. */

    return value;
}

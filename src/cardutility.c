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
static int isrand_init = 0;

int cards_played(const struct Hand *hand)
{
	int count = 0;
    for (int i = 0; i < hand->ncards; i++)
        if (hand->isplayed[i] == 1)
            count++;
    return count;
}

int card_compare(const struct Card *c1, const struct Card *c2)
{
	if (c1->number < c2->number)
		return -1;
	else if (c1->number > c2->number)
		return 1;
	else
		return (c1->suit == c2->suit) ? 0 : c1->suit - c2->suit;
}

void fill_linked_hand(struct LinkedHand *l_hand, const struct Hand *hand)
{
	struct CardNode *prev = l_hand->node, *curr;	/* Assumes l_hand->node exists, but is uninitialized */
	prev->card = &hand->cards[0];

	for (int i = 1; i < hand->ncards; i++) {
		curr = malloc(sizeof(struct CardNode));
		curr->card = &hand->cards[i];		/* Set card in new node; does NOT allocate new memory for Card */
		prev->next = curr;
		curr->next = NULL;					/* Next MUST be set to NULL for free() to work */
		prev = curr;
	}
	l_hand->ncards = hand->ncards;
}

struct Hand * fopen_hand(FILE *file, int nhands)
{
    struct Hand *hands;
    struct Card *cards[nhands];
    int ncards[nhands], counter[nhands];    /* Number of cards in each hand; counter for error-checking */
    int card_val, c;
    
    hands = malloc(nhands * sizeof(struct Hand));
    /* Go through each line until the newline */
    for (int i = 0; i < nhands; i++) {
        fscanf(file, "%3d", &ncards[i]);    /* Max # of cards = 999 */
        cards[i] = malloc(ncards[i] * sizeof(struct Card));
        counter[i] = 0;
        
        while ((c = fgetc(file)) != '\n') {
            fscanf(file, "%3d", &card_val);
            cards[i][counter[i]].number = card_val % 13 + 1;
            cards[i][counter[i]].suit = card_val / 13 ;
            counter[i]++;
            printf("%d: %d\n", counter[i], card_val);
        }
        if (ncards[i] != counter[i]) {
        	printf("Hey, no cheating.\n");
        	exit(EXIT_FAILURE);
        }
        hands[i].cards = cards[i];
        hands[i].ncards = ncards[i];
        hands[i].isplayed = malloc(ncards[i] * sizeof(int));
    }
    return hands;
}

void free_hand(struct Hand *hand, int do_free_hand)
{
	free(hand->isplayed);
	free(hand->cards);
	if (do_free_hand)
		free(hand);
}

void free_linked_hand(struct LinkedHand *hand, int do_freecards)
{
	struct CardNode *node, *prev;

	node = hand->node;
	while (node != NULL) {					/* Go through nodes, freeing current one and going on to the next */
		if (do_freecards)
			free(node->card);
		prev = node;
		node = node->next;
		free(prev);
	}
	free(hand);
}

void fsave_linked_hand(const struct LinkedHand *hand, FILE *file)
{
    struct CardNode *node = hand->node;

    /* Write all cards on one line, space-separated */
    fprintf(file, "%d", hand->ncards);           	/* Put ncards as first number */
    for (; node != NULL; node = node->next)
        fprintf(file, " %d", get_card_value(node->card));
    fprintf(file, "\n");                          	/* Newline at the end */
}

struct Hand * gen_ordered_deck()
{
    struct Hand *hand = malloc(sizeof(struct Hand));
    struct Card *cards = malloc(52 * sizeof(struct Card));
    int *isplayed;

    for (int i = 0; i < 52; i++) {			/* Populate the deck */
        cards[i].number = i % 13 + 1;
        cards[i].suit = i / 13;
    }
    hand->ncards = 52;
    hand->cards = cards;					/* Remember to free() this... */
    isplayed = malloc(hand->ncards * sizeof(int));
    memset(isplayed, 0, hand->ncards * sizeof(int));
    hand->isplayed = isplayed;           	/* ...and this! */

    return hand;
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

char * get_card_encoding(const struct Card *card)
{
	char *enc = malloc(5 * sizeof(char));
	strcpy(enc, CARD_UTF_PREFIX);
	strcat(enc, CARD_SUFFIXES[card->suit]);
	enc[3] += card->number - 1;				/* Some character addition */

	return enc;
}

/* Note: if there is an error, this function returns a null string. */
char * get_card_name(const struct Card *card)
{
    char *name = malloc(20 * sizeof(char));
    switch (card->number) {
        case 1: strcpy(name, "Ace");    break;
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
    switch (card->suit) {
        case CLUB:      strcat(name, " of Clubs");     break;
        case DIAMOND:   strcat(name, " of Diamonds");  break;
        case HEART:     strcat(name, " of Hearts");    break;
        case SPADE:     strcat(name, " of Spades");    break;
        default: return '\0';
    }
    return name;
}

int get_card_value(const struct Card *card)
{
    int value = 0;
    value += card->number;
    value += card->suit * 13;				/* Clubs are 0-12, Diamonds are 13-25, etc. */

    return value;
}

void linked_hand_add(struct LinkedHand *hand, struct CardNode *node)
{
	struct CardNode *base = hand->node;

	while (base->next != NULL)				/* Go to the end of the list (not bothering to use one-line for loop) */
		base = base->next;
	base->next = node;
}

void shuffle_hand(struct Hand *hand, int ntimes)
{
	int swap1, swap2, ncards = hand->ncards;
	struct Card temp, *cards = hand->cards;
	printf("NUM: %d\n", ncards);

	if (!isrand_init) {
		srand(time(NULL));
		isrand_init = 1;
	}
	for (int i = 0; i < ntimes; i++) {
		swap1 = rand() % ncards;			/* Thankfully, we have ncards */
		do {
			swap2 = rand() % ncards;
		} while (swap1 == swap2);

		temp = cards[swap1];
		cards[swap1] = cards[swap2];
		cards[swap2] = temp;
	}
}

struct Hand * split_hand(struct Hand *hand, int nhands)
{
	struct Hand *hands = malloc(nhands * sizeof(struct Hand));		/* Remember to free this! */
	int counter = 0;

	for (int i = 0; i < nhands; i++) {
		/* Assign to hand[i] an array of size 52 / nhands. Note that this
		 * uses the cards/isplayed in the Hand parameter - no new memory is allocated
         * EXCEPT for the array of Hands themselves. */
		hands[i].cards = &hand->cards[52 / nhands * i];
        hands[i].isplayed = &hand->isplayed[52 / nhands * i];
		hands[i].ncards = 52 / nhands;
		counter += hands[i].ncards;
	}
	hands[nhands - 1].ncards += 52 - counter;	    /* Adjust for possible remainder */

	return hands;
}

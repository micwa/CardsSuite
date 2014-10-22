#include "cardutility.h"
#include "carddefs.h"
#include "structhelper.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

/* Define constants from carddefs.h */
const char *CARD_UTF_PREFIX = "\xf0\x9f";
const char *CARD_SUFFIXES[4] = { "\x83\x91", "\x83\x81", "\x82\xb1" , "\x82\xa1" };
const char *CARD_BACK = "\xf0\x9f\x82\xa0";

char *g_card_encs[52] = { NULL };

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
	struct CardNode *prev, *curr;

	/* If l_hand->node exists, overwrites it */
    l_hand->node = card_node_create(&hand->cards[0], NULL);
    prev = l_hand->node;

	for (int i = 1; i < hand->ncards; i++) {
		curr = card_node_create(&hand->cards[i], NULL);
		prev->next = curr;
		prev = curr;
	}
	l_hand->ncards = hand->ncards;
}

struct Hand * fopen_hand(FILE *file, int nhands)
{
    struct Hand *hands;
    struct Card *cards[nhands];
    int ncards[nhands], counter[nhands];    /* Number of cards in each hand; counter for error-checking */
    int card_val, c, is_hand = 0;
    
    hands = malloc(nhands * sizeof(struct Hand));
    /* Go through each line until the newline */
    for (int i = 0; i < nhands; i++) {
        fscanf(file, "%3d", &ncards[i]);    /* Max # of cards = 999 */
        if (ncards[i] < 0) {                /* Setting is_hand flag when applicable */
            is_hand = 1;
            ncards[i] = 0 - ncards[i];
        }
        cards[i] = malloc(ncards[i] * sizeof(struct Card));
        counter[i] = 0;
        
        while ((c = fgetc(file)) != '\n') {
            fscanf(file, "%3d", &card_val);
            cards[i][counter[i]].number = card_val % 13 + 1;
            cards[i][counter[i]].suit = card_val / 13 ;
            counter[i]++;
        }
        if (ncards[i] != counter[i]) {
        	printf("Hey, no cheating.\n");
        	exit(EXIT_FAILURE);
        }
        hands[i].cards = cards[i];
        hands[i].ncards = ncards[i];
        
        if (is_hand) {                      /* Read the next line if the saved hand was a Hand */
            int *isplayed = malloc(ncards[i] * sizeof(int)), count2 = 0;
            fscanf(file, "%1d", &isplayed[count2++]);
            
            while ((c = fgetc(file)) != '\n')
                fscanf(file, "%1d", &isplayed[count2++]);
            
            if (ncards[i] != count2) {
                printf("Hey, no cheating.\n");
                exit(EXIT_FAILURE);
            }
            hands[i].isplayed = isplayed;
        } else {
            hands[i].isplayed = malloc(ncards[i] * sizeof(int));
            memset(hands[i].isplayed, 0, ncards[i]);
        }
    }
    return hands;
}

void free_card_encs()
{
	if (g_card_encs[0] == NULL)				/* Technically, all card_encs should be init.'d at the same time */
		return;
	for (int i = 0; i < 52; i++)
		free(g_card_encs[i]);
}

void fsave_hand(const struct Hand *hand, FILE *file)
{
    /* Write the cards on the first line, isplayed on the line */
    fprintf(file, "%d", 0 - hand->ncards);  /* Indicate that this is not a LinkedHand */
    
    for (int i = 0; i < hand->ncards; i++)
        fprintf(file, " %d", get_card_value(&hand->cards[i]));
    fprintf(file, "\n");
    for (int i = 0; i < hand->ncards; i++)
        fprintf(file, " %d", hand->isplayed[i]);     /* Leading space shouldn't matter */
    fprintf(file, "\n");
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
    struct Hand *hand = hand_create(52);	/* Remember to free() this (as with all other *_create() functions) */
    struct Card *cards = hand->cards;

    for (int i = 0; i < 52; i++) {			/* Populate the deck */
        cards[i].number = i % 13 + 1;
        cards[i].suit = i / 13;
    }
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
    suit = rand() / 13;
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
    value += card->number - 1;
    value += card->suit * 13;				/* Clubs are 0-12, Diamonds are 13-25, etc. */

    return value;
}

int get_next_unplayed(struct Hand *hand, int index, int do_loop)
{
	if (index < 0)							/* Set to 0 for invalid indices (whether intended or not) */
		index = 0;

	for (int i = index; i < hand->ncards; i++)
		if (hand->isplayed[i] == 0)
			return i;

	if (do_loop)							/* Looping from 0 to index - 1 */
		for (int i = 0; i < index; i++)
			if (hand->isplayed[i] == 0)
				return i;
	return -1;
}

void init_card_encs()
{
	struct Hand *hand = gen_ordered_deck();	/* Do this because get_card_encoding doesn't accept ints */
	for (int i = 0; i < 52; i++)
		g_card_encs[i] = get_card_encoding(&hand->cards[i]);

	free_hand(hand, 1);
}

void linked_hand_add(struct LinkedHand *hand, struct CardNode *node)
{
	struct CardNode *base = hand->node;

	if (base != NULL) {						/* First check if base is not NULL to begin with */
		while (base->next != NULL)
			base = base->next;
		base->next = node;
	} else
		hand->node = node;

	hand->ncards++;
	base = node;
	for (; base->next != NULL; base = base->next)	/* If the node has a next node itself */
		hand->ncards++;
}

struct Card * linked_hand_get_card(struct LinkedHand *hand, int index)
{
	if (index >= hand->ncards || index < 0)
		return NULL;

	struct CardNode *base = hand->node;
	for (int i = 0; i < index; i++)
		base = base->next;

	return base->card;
}

struct CardNode * linked_hand_get_node(struct LinkedHand *hand, int index)
{
	if (index >= hand->ncards || index < 0)
		return NULL;

	struct CardNode *base = hand->node;
	for (int i = 0; i < index; i++)
		base = base->next;

	return base;
}

struct CardNode * linked_hand_remove(struct LinkedHand *hand, int index, int remove_all)
{
	if (index == 0) {						/* Must change the value of the head pointer itself */
		struct CardNode *ret = hand->node;

		if (remove_all) {
			hand->node = NULL;
			hand->ncards = 0;
		} else {
			hand->node = hand->node->next;
			hand->ncards--;
		}
		return ret;
	} else {
		struct CardNode *prev, *curr = hand->node;

		for (int i = 0; i < index; i++) {
			prev = curr;
			curr = curr->next;
		}
		if (remove_all) {
			prev->next = NULL;
			hand->ncards = index;
		} else {
			prev->next = curr->next;			/* Assume that the end node is always NULL, so don't need to check for ncards - 1 case */
			hand->ncards--;
		}
		return curr;
	}
}

void shuffle_hand(struct Hand *hand, int ntimes)
{
	int swap1, swap2, ncards = hand->ncards;
	struct Card temp, *cards = hand->cards;

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

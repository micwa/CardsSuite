#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "cardutility.h"
#include "carddefs.h"

static const int SHUFFLE_AMOUNT = 1000;

struct Card gen_random_card()
{
    int num, suit;
    struct Card card;

    srand(time(NULL));
    num = rand() % 13 + 1;
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

    for (int i = 0; i < 52; i++) {
        cards[i].number = i % 13 + 1;
        cards[i].suit = i / 13;
    }
    for (int i = 0; i < SHUFFLE_AMOUNT; i++) {
        srand(time(NULL));
        swap1 = rand() % 52;
        do {
            swap2 = rand() % 52;
        } while (swap1 == swap2);

        temp_card = cards[swap1];
        cards[swap1] = cards[swap2];
        cards[swap2] = temp_card;
    }
    
    hand.ncards = 52;
    hand.cards = cards;

    return hand;
}

int get_card_value(struct Card card)
{
    int value = 0;
    value += card.number;
    value += card.suit * 13;

    return value;
}

char * get_card_name(struct Card card)
{
    char *name = malloc(15 * sizeof(char));
    switch (card.number) {
        case 1: strcpy(name, "One");    break;
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

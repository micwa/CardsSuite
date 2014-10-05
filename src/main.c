#include <stdio.h>
#include "carddefs.h"
#include "cardutility.h"

void testHand();

int main(int argc, char *argv[])
{
    /* Starting game */
    if (argc == 1) {
        printf("No card game specified.\n");
    } else if (argc == 2) {
        printf("Now playing %s...\n", argv[1]);
    } else {
        printf("Too many arguments!");
    }

    /* Testing */
    testHand();
    return 0; 
}

void testHand()
{
    struct Card *cards = gen_random_deck().cards;
    for (int i = 0; i < 52; i++, cards++) {
        printf("%s\n", get_card_name(*cards));
    }
}

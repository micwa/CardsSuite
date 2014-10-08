#include <stdio.h>
#include <string.h>

#include "wargame.h"

#include "carddefs.h"
#include "cardutility.h"
#include "stdlib.h"

void printstuff();

int main(int argc, char *argv[])
{
	printstuff();
    /* Start game, and hand all control to that game */
    if (argc == 1) {
        printf("No card game specified.\n");
    } else if (argc == 2) {
        if (strcmp(argv[1], "war") == 0)
        	war();
        else
        	printf("Not a valid game");
    } else {
        printf("Too many arguments!");
    }

    return 0; 
}

void print_linkedlist()
{
	struct LinkedHand hand;
	hand.node = malloc(sizeof(struct LinkedHand));
	struct Hand h = gen_random_deck(1000);
	fill_linked_hand(&hand, &h);
	struct CardNode *curr = hand.node;

	do {
		printf("%s\n", get_card_name(curr->card));
	} while ((curr = curr->next) != NULL);
}

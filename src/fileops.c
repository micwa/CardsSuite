#include "fileops.h"
#include "carddefs.h"
#include "cardutility.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

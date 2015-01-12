#include "fileops.h"
#include "carddefs.h"
#include "cardutility.h"
#include "structhelper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Hand ** fopen_hand(FILE *file, int nhands)
{
    struct Hand **hands = malloc(nhands * sizeof(struct Hand *));
    int ncards[nhands], count[nhands];      /* Number of cards in each hand; counter for error-checking */
    int card_val, is_hand = 0;

    /* Go through each line until the newline */
    for (int i = 0; i < nhands; i++) {
        fscanf(file, "%3d", &ncards[i]);    /* Max # of cards = 999 */
        if (ncards[i] < 0) {                /* Set is_hand flag when applicable */
            is_hand = 1;
            ncards[i] = 0 - ncards[i];
        }
        hands[i] = hand_create(ncards[i]);
        count[i] = 0;

        while (fgetc(file) != '\n') {
            fscanf(file, "%3d", &card_val);
            hands[i]->cards[count[i]].number = card_val % 13 + 1;
            hands[i]->cards[count[i]].suit = card_val / 13 ;
            count[i]++;

            if (ncards[i] < count[i]) {
                printf("Hey, no cheating.\n");
                exit(EXIT_FAILURE);
            }
        }

        if (is_hand) {                      /* Read the next line if the saved hand was a Hand */
            int count2 = 0;
            fscanf(file, "%1d", &hands[i]->isplayed[count2++]);

            while (fgetc(file) != '\n') {
                fscanf(file, "%1d", &hands[i]->isplayed[count2++]);

                if (ncards[i] < count2) {
                    printf("Hey, no cheating.\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    return hands;
}

void fsave_hand(const struct Hand *hand, FILE *file)
{
    /* Write the cards on the first line, isplayed on the line */
    fprintf(file, "%d", 0 - hand->ncards);          /* Indicate that this is not a LinkedHand */

    for (int i = 0; i < hand->ncards; i++)
        fprintf(file, " %d", get_card_value(&hand->cards[i]));
    fprintf(file, "\n");
    for (int i = 0; i < hand->ncards; i++)
        fprintf(file, " %d", hand->isplayed[i]);    /* Leading space shouldn't matter */
    fprintf(file, "\n");
}

void fsave_linked_hand(const struct LinkedHand *hand, FILE *file)
{
    struct CardNode *node = hand->node;

    /* Write all cards on one line, space-separated */
    fprintf(file, "%d", hand->ncards);              /* Put ncards as first number */
    for (; node != NULL; node = node->next)
        fprintf(file, " %d", get_card_value(node->card));
    fprintf(file, "\n");                            /* Newline at the end */
}

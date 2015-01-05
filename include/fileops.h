#ifndef FILEOPS_H_
#define FILEOPS_H_

#include "carddefs.h"
#include <stdio.h>

/* Creates an array of hands from the hand(s) stored in the FILE; the user must specify
 * the number of hands (i.e. sets of lines to read).
 * Distinguishes between regular Hands and LinkedHands based on the fsave_hand() and
 * fsave_linked_hand(); for LinkedHands, sets isplayed to all 0s. */
struct Hand ** fopen_hand(FILE *file, int nhands);

/* Saves the given Hand to the specified file. This function should write the hand in
 * two lines, differing from writing a LinkedHand so that fopen_hand() can distinguish
 * between them. */
void fsave_hand(const struct Hand *hand, FILE *file);

/* Saves the given LinkedHand to the specified file. This function should write the whole
 * hand on one line, and append a newline character at the end. */
void fsave_linked_hand(const struct LinkedHand *hand, FILE *file);

#endif /* FILEOPS_H_ */

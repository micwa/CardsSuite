#ifndef SOLITSTACK_H_
#define SOLITSTACK_H_

#include "solitdefs.h"

/* Create a SolitStack with the specified size. Sets the curr_index to -1, and all
 * elements to NULL. */
struct SolitStack * stack_create(int size);

/* Free()s the array of the elements in the stack (but not the elements themselves) */
void SolitStack * stack_destroy(struct SolitStack *stack);

/* Returns the top element in the stack */
struct SolitMove * stack_peek(struct SolitStack *stack);

/* Returns and removes the top element in the stack */
struct SolitMove * stack_pop(struct SolitStack *stack);

/* Pushes the given move onto the stack; if the stack is full, overwrites the
 * oldest element in the stack. */
void stack_push(struct SolitStack *stack, struct SolitMove *move);

#endif /* SOLITSTACK_H_ */

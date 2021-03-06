#ifndef SOLITSTACK_H_
#define SOLITSTACK_H_

#include "solitdefs.h"

/* Create a SolitStack with the specified size. Sets the curr_index to -1, and all
 * elements to NULL. */
struct SolitStack * stack_create(int size);

/* Free()s the array of the elements in the stack (and the elements themselves) */
void stack_destroy(struct SolitStack *stack);

/* Returns a pointer to the top element in the stack, or NULL if the stack is empty */
struct SolitMove * stack_peek(struct SolitStack *stack);

/* Returns (the pointer to) and removes the top element in the stack, or NULL if
 * the stack is empty */
struct SolitMove * stack_pop(struct SolitStack *stack);

/* Pushes a copy of the given move onto the stack; if the stack is full, overwrites
 * the oldest element in the stack. */
void stack_push(struct SolitStack *stack, struct SolitMove *move);

#endif /* SOLITSTACK_H_ */

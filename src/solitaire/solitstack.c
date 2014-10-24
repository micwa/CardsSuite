#include "solitstack.h"
#include "solitdefs.h"

#include <stdlib.h>

struct SolitStack * stack_create(int size)
{
	struct SolitStack *stack = malloc(sizeof(struct SolitStack));
	stack->size = 0;
	stack->curr_index = -1;
	stack->elems = malloc(size * sizeof(struct SolitMove));
	for (int i = 0; i < size; i++)			/* Set the MoveTypes to NONE */
		stack->elems[i].type = NONE;

	return stack;
}

void stack_destroy(struct SolitStack *stack)
{
	free(stack->elems);
	free(stack);
}

struct SolitMove * stack_peek(struct SolitStack *stack)
{
	return &stack->elems[stack->curr_index];
}

struct SolitMove * stack_pop(struct SolitStack *stack)
{
	struct SolitMove *move = &stack->elems[stack->curr_index];

	/* If the MoveType is NONE, returns NULL. Otherwise, reset the
	 * element to MoveType.NONE, and decrement curr_index properly. */
	if (move->type == NONE)
		return NULL;

	stack->elems[stack->curr_index].type = NONE;
	stack->curr_index--;
	if (stack->curr_index < 0)
		stack->curr_index = stack->size - 1;

	return move;
}

void stack_push(struct SolitStack *stack, struct SolitMove *move)
{
	stack->curr_index++;					/* Will increment to 0 if curr_index = -1 */
	if (stack->curr_index >= stack->size)	/* If curr_index "overflows", it's reset to 0; it will ALWAYS be between 0 and size - 1 */
		stack->curr_index = 0;

	stack->elems[stack->curr_index] = *move;
}

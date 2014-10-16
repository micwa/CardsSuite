#ifndef SOLITUTIL_H_
#define SOLITUTIL_H_

enum MoveType { WASTE_TO_TBL,
    WASTE_TO_TBL_EMPTY,                     /* From waste to an empty tableau pile */
    WASTE_TO_FDTION, 
    TBL_TO_TBL,                             /* Any amount of tableau cards to another non-empty tableau pile */
    TBL_TO_TBL_EMPTY,
    TBL_TO_FDTION,
    TBL_MANY_TO_FDTION,						/* Always invalid; next three are always true */
    FLIP_STOCK, 
    FLIP_TBL,
    NONE
};

/* Returns 1 is the moving card src to dest is a valid move, and -1 if not.
 * For FLIP_STOCK and FLIP_TBL, this function will immediately return 1.
 * Returns 0 on if type if not a valid MoveType. */
int is_valid_move(enum MoveType type, struct Card *src, struct Card *dest);

#endif /* SOLITLUTIL_H_ */

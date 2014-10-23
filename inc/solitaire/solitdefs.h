#ifndef SOLITDEFS_H_
#define SOLITDEFS_H_

enum MoveType { WASTE_TO_TBL,
    WASTE_TO_TBL_EMPTY,                     /* From waste to an empty tableau pile */
    WASTE_TO_FDTION,
    TBL_TO_TBL,                             /* TBL = any amount of tableau cards, TO_TBL = non-empty pile */
    TBL_TO_TBL_EMPTY,
    TBL_SINGLE_TO_FDTION,
    TBL_TO_FDTION,					        /* Always invalid; next two are always true */
    FLIP_STOCK,
    NONE
};

struct SolitMove {
	enum MoveType type;
	void *src;								/* Source and destination */
	void *dest;
	int *num;								/* A number used to hold the col/row/whatever needs to be held */
};

/* A stack implemented as an array of SolitMove pointers with a fixed size */
struct SolitStack {
	int size;
	int curr_index;
	struct SolitMove **elems;
};

#endif /* SOLITDEFS_H_ */

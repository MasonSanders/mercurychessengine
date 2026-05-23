#ifndef __MOVE_H__
#define __MOVE_H__

#include "MoveType.h"


struct Move {
    int from;
    int to;

    MoveType type;

    int promotion_piece = -1;
}

#endif
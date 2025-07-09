#ifndef MUTILS_H
#define MUTILS_H

#include "stdbool.h"

float maxf(float a, float b);
float minf(float a, float b);

typedef struct MoveActions {
    bool move_up;
    bool move_down;
    bool move_left;
    bool move_right;
} MoveActions;

#endif
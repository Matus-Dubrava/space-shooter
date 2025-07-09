#ifndef MUTILS_H
#define MUTILS_H

#include "stdbool.h"
typedef struct MoveActions {
    bool move_up;
    bool move_down;
    bool move_left;
    bool move_right;
} MoveActions;

typedef struct ScreenCollisions {
    bool left_side;
    bool right_side;
    bool top_side;
    bool bottom_side;
} ScreenCollisions;

float maxf(float a, float b);
float minf(float a, float b);

void check_screen_collisions(float x,
                             float y,
                             float x_offset,
                             float y_offset,
                             ScreenCollisions* screen_collisions_out);

#endif
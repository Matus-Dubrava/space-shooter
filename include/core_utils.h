#ifndef MUTILS_H
#define MUTILS_H

#include "stdbool.h"

typedef enum Direction {
    DIRECTION_DOWN,
    DIRECTION_UP,
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
} Direction;

typedef struct Actor Actor;
typedef struct Actors Actors;

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

typedef struct ClosestEnemy {
    Actor* actor;
    float dist;
} ClosestEnemy;

float maxf(float a, float b);

float minf(float a, float b);

/**
 * Get squared distance between actors.
 * @param actor actor 1
 * @param actor actor 2
 * @return Squared distance between actors
 */
float get_squared_dist(const Actor* actor, const Actor* other);

void check_screen_collisions(float x,
                             float y,
                             float x_offset,
                             float y_offset,
                             ScreenCollisions* screen_collisions_out);

void find_closest_enemy(const Actor* player,
                        const Actors* enemies,
                        ClosestEnemy* target_out);

#endif
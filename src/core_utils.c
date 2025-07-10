#include "core_utils.h"
#include "actor.h"
#include "core_config.h"
#include "limits.h"

float maxf(float a, float b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

float minf(float a, float b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

void check_screen_collisions(float x,
                             float y,
                             float x_offset,
                             float y_offset,
                             ScreenCollisions* screen_collisions_out) {
    screen_collisions_out->right_side = x + x_offset >= SCREEN_WIDTH;
    screen_collisions_out->left_side = x - x_offset <= 0;
    screen_collisions_out->bottom_side = y + y_offset >= SCREEN_HEIGHT;
    screen_collisions_out->top_side = y - y_offset <= 0;
}

float get_squared_dist(Actor* actor, Actor* other) {
    return (actor->pos.x - other->pos.x) * (actor->pos.x - other->pos.x) +
           (actor->pos.y - other->pos.y) * (actor->pos.y - other->pos.y);
}

void find_closest_enemy(Actor* player,
                        Actors* enemies,
                        ClosestEnemy* target_out) {
    Actor* target = NULL;
    size_t min_dist = LONG_MAX;

    for (size_t i = 0; i < enemies->len; ++i) {
        if (enemies->items[i]->is_valid) {
            const float dist = get_squared_dist(player, enemies->items[i]);
            if (!target || dist < min_dist) {
                min_dist = dist;
                target = enemies->items[i];
            }
        }
    }

    target_out->actor = target;
    target_out->dist = min_dist;
}
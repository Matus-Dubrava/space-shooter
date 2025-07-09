#include "core_utils.h"
#include "core_config.h"

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
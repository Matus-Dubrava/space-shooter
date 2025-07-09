#ifndef XP_BOX_H
#define XP_BOX_H

#include "raylib.h"
#include "stdint.h"
#include "stdlib.h"

typedef struct Actor Actor;

typedef struct XPBox {
    Vector2 pos;
    Vector2 size;
    float xp_rewarded;
    uint8_t lifetime;  // in seconds
    float time_remaining;
    bool is_valid;
    float down_speed;
    float right_speed;
    float speed_damping;
} XPBox;

typedef struct XPBoxes {
    XPBox** items;
    size_t capacity;
    size_t len;
} XPBoxes;

XPBox* XP_create_box_p(Vector2 pos,
                       Vector2 size,
                       float xp_rewarded,
                       uint8_t lifetime);

void XP_move_box(XPBox* xp_box);

void XP_handle_collision_with_player(Actor* player, XPBox* xp_box);

void XP_update_lifetime_timer(XPBox* xp_box);

XPBoxes* XP_create_boxes_p(size_t capacity);

void XP_draw_boxes(XPBoxes* boxes);

#endif
#ifndef XP_BOX_H
#define XP_BOX_H

#include "raylib.h"
#include "stdint.h"

typedef struct Actor Actor;

typedef struct XPBox {
    Vector2 pos;
    Vector2 size;
    float xp_rewarded;
    uint8_t lifetime;  // in seconds
    uint8_t time_remaining;
    bool is_valid;
    float down_speed;
    float right_speed;
    float speed_damping;
} XPBox;

XPBox* XP_create_xp_box(Vector2 pos,
                        Vector2 size,
                        float xp_rewarded,
                        uint8_t lifetime);

void XP_move_xp_box(XPBox* xp_box);

void XP_handle_collision_with_player(Actor* player, XPBox* xp_box);

void XP_update_lifetime_timer(XPBox* xp_box);

#endif
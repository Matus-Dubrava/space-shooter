#include "xp_box.h"
#include "actor.h"
#include "core_config.h"
#include "stdio.h"
#include "stdlib.h"

XPBox* XP_create_box_p(Vector2 pos,
                       Vector2 size,
                       float xp_rewarded,
                       uint8_t lifetime) {
    XPBox* xp_box = malloc(sizeof(XPBox));
    if (!xp_box) {
        perror("failed to allocate memory for XP box");
        return NULL;
    }

    xp_box->pos = pos;
    xp_box->size = size;
    xp_box->xp_rewarded = xp_rewarded;
    xp_box->lifetime = lifetime;
    xp_box->time_remaining = lifetime;
    xp_box->is_valid = true;

    xp_box->right_speed = (float)GetRandomValue(-200, 200) / 100;
    xp_box->down_speed = (float)GetRandomValue(200, 300) / 100;
    xp_box->speed_damping = (float)GetRandomValue(9500, 9999) / 10000;

    return xp_box;
}

void XP_move_box(XPBox* xp_box) {
    // horizontal movement
    xp_box->right_speed = xp_box->right_speed * xp_box->speed_damping;
    xp_box->pos.x += xp_box->right_speed;

    // vertical movement
    xp_box->down_speed = xp_box->down_speed * xp_box->speed_damping;
    xp_box->pos.y += xp_box->down_speed;
}

void XP_handle_collision_with_player(Actor* player, XPBox* xp_box) {
    if (xp_box->is_valid) {
        if (CheckCollisionCircleRec(player->pos, player->capsule_radius,
                                    (Rectangle){.x = xp_box->pos.x,
                                                .y = xp_box->pos.y,
                                                .width = xp_box->size.x,
                                                .height = xp_box->size.y})) {
            xp_box->is_valid = false;
            player->xp += xp_box->xp_rewarded;
        }
    }
}

void XP_update_lifetime_timer(XPBox* xp_box) {
    if (xp_box->is_valid) {
        xp_box->time_remaining -= 1 / TARGET_FPS;
        if (xp_box->time_remaining <= 0) {
            xp_box->is_valid = false;
        }
    }
}

XPBoxes* XP_create_boxes_p(size_t capacity) {
    XPBoxes* boxes = malloc(sizeof(XPBoxes));
    if (!boxes) {
        perror("failed to allocate memory for xp boxes");
        return NULL;
    }

    XPBox** items = malloc(capacity * sizeof(XPBox*));
    if (!items) {
        perror("failed to allocate memory for xp box items");
        free(boxes);
        return NULL;
    }

    boxes->items = items;
    boxes->len = 0;
    boxes->capacity = capacity;
    return boxes;
}

void XP_draw_boxes(XPBoxes* boxes) {
    for (size_t i = 0; i < boxes->len; ++i) {
        if (boxes->items[i]->is_valid) {
            DrawRectangleV(boxes->items[i]->pos, boxes->items[i]->size, WHITE);
        }
    }
}
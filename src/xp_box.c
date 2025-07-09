#include "xp_box.h"
#include "actor.h"
#include "core_config.h"
#include "core_utils.h"
#include "stdio.h"
#include "stdlib.h"

XPBox* XP_create_box_p(Vector2 pos,
                       Vector2 size,
                       float xp_rewarded,
                       uint8_t lifetime_seconds) {
    XPBox* box = malloc(sizeof(XPBox));
    if (!box) {
        perror("failed to allocate memory for XP box");
        return NULL;
    }

    box->pos = pos;
    box->size = size;
    box->xp_rewarded = xp_rewarded;
    box->lifetime_seconds = lifetime_seconds;
    box->time_remaining = lifetime_seconds;
    box->is_valid = true;

    box->right_speed = (float)GetRandomValue(-200, 200) / 100;
    box->down_speed = (float)GetRandomValue(200, 300) / 100;
    box->speed_damping = (float)GetRandomValue(9750, 9999) / 10000;

    return box;
}

void XP_move_box(XPBox* box) {
    ScreenCollisions screen_collisions = {0};
    float x_offset = box->size.x + box->right_speed;
    float y_offset = box->size.y + box->down_speed;
    check_screen_collisions(box->pos.x, box->pos.y, x_offset, y_offset,
                            &screen_collisions);

    // horizontal movement
    box->right_speed = box->right_speed * box->speed_damping;
    if ((box->right_speed > 0 && !screen_collisions.right_side) ||
        (box->right_speed < 0 && !screen_collisions.left_side)) {
        box->pos.x += box->right_speed;
    }

    // vertical movement
    box->down_speed = box->down_speed * box->speed_damping;
    if ((box->down_speed > 0 && !screen_collisions.bottom_side) ||
        (box->down_speed > 0 && !screen_collisions.top_side)) {
        box->pos.y += box->down_speed;
    }
}

void XP_handle_collision_with_player(Actor* player, XPBox* box) {
    if (box->is_valid) {
        if (CheckCollisionCircleRec(player->pos, player->capsule_radius,
                                    (Rectangle){.x = box->pos.x,
                                                .y = box->pos.y,
                                                .width = box->size.x,
                                                .height = box->size.y})) {
            box->is_valid = false;
            player->xp += box->xp_rewarded;
        }
    }
}

void XP_update_lifetime_timer(XPBox* box) {
    if (box->is_valid) {
        box->time_remaining -= 1.f / TARGET_FPS;
        if (box->time_remaining <= 0) {
            box->is_valid = false;
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
#ifndef ACTOR_H
#define ACTOR_H

#include "raylib.h"
#include "stdlib.h"

typedef struct Actor {
    Vector2 pos;
    float speed;
    float capsule_radius;
    float down_speed;
    float right_speed;
    float speed_damping;
    float acceleration;
    size_t shoot_action_delay_frames;
    size_t shoot_action_delay_remaining_frames;
    bool ongoing_shoot_action_delay;
    int health;
    int max_health;

    // indicates whehether actor is still valid or if it should be treated as if
    // it was destroyed
    bool is_valid;
} Actor;

typedef struct Projectile {
    Vector2 pos;
    float speed;
    float capsule_radius;
    float down_speed;
    float right_speed;
    float speed_damping;
    float acceleration;
    float damage;

    // indicates whehether actor is still valid or if it should be treated as if
    // it was destroyed
    bool is_valid;
} Projectile;

Projectile* create_projectile(Vector2 pos,
                              float speed,
                              float capsule_size,
                              float down_speed,
                              float right_speed,
                              float speed_damping,
                              float acceleration,
                              float damage,
                              bool is_valid);

#endif
#ifndef ACTOR_H
#define ACTOR_H

#include "raylib.h"
#include "stdint.h"
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
    float health;
    float max_health;
    float health_regen_rate;  // per 60 frames
    float shield;
    float max_shield;
    float shield_regen_rate;  // per 60 frames
    size_t xp;
    size_t xp_reward;
    uint16_t level;
    float levelup_xp_required;

    // iframes
    bool iframes_enabled;  // can this actor use iframes
    bool iframes_active;   // are iframes currently active
    int iframes_remainig;
    int n_iframes;

    // indicates whehether actor is still valid or if it should be treated as if
    // it was destroyed
    bool is_valid;
} Actor;

#endif
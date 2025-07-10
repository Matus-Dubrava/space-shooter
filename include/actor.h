#ifndef ACTOR_H
#define ACTOR_H

#include "raylib.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

typedef struct DebugCtx DebugCtx;

typedef struct Actor {
    Vector2 pos;
    float speed;
    float capsule_radius;
    float down_speed;
    float right_speed;
    float speed_damping;
    float acceleration;
    float health;
    float max_health;
    float health_regen_rate;  // per 60 frames
    float shield;
    float max_shield;
    float shield_regen_rate;  // per 60 frames
    size_t xp;
    size_t xp_reward;
    uint16_t level;
    size_t levelup_xp_required;
    bool is_player;

    // actor level projectile settings
    uint16_t shoot_action_delay_frames;
    uint16_t shoot_action_delay_remaining_frames;
    bool ongoing_shoot_action_delay;
    bool multishot_enabled;
    uint16_t n_projs;

    // iframes
    bool iframes_enabled;  // can this actor use iframes
    bool iframes_active;   // are iframes currently active
    int iframes_remainig;
    int n_iframes;

    // indicates whehether actor is still valid or if it should be treated as if
    // it was destroyed
    bool is_valid;
} Actor;

typedef struct Actors {
    Actor** items;
    size_t capacity;
    size_t len;
} Actors;

Actors* A_create_actors_p(size_t capacity, DebugCtx* debug_ctx);

Actor* A_create_test_enemy_p(DebugCtx* debug_ctx);

int A_add_actor(Actors* actors, Actor* actor, DebugCtx* debug_ctx);

#endif
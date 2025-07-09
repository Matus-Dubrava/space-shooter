#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "raylib.h"
#include "stdlib.h"

typedef struct Actor Actor;
typedef struct XPBox XPBox;
typedef struct XPBoxes XPBoxes;
typedef struct DebugCtx DebugCtx;

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

typedef struct Projectiles {
    Projectile** items;
    size_t capacity;
    size_t len;
} Projectiles;

typedef struct ProjectileInitArgs {
    Vector2 pos;
    float speed;
    float capsule_size;
    float down_speed;
    float right_speed;
    float speed_damping;
    float acceleration;
    float damage;
} ProjectileInitArgs;

Projectile* PROJ_create_projectile_p(ProjectileInitArgs* args,
                                     DebugCtx* debug_ctx);

Projectiles* PROJ_create_projectiles_p(size_t capacity);

// Free invalidated projectiles.
// Return 0 if succesful, -1 on error.
int PROJ_clear_invalid_projectiles(Projectiles* projectiles,
                                   DebugCtx* debug_ctx);

void PROJ_draw_projectiles(Projectiles* projectiles, Color color);

void PROJ_handle_projectile_collision(Actor* actor,
                                      Projectiles* projectiles,
                                      XPBoxes* xp_boxes,
                                      DebugCtx* debug_ctx);

void PROJ_register(Actor* actor,
                   Projectiles* projectiles,
                   bool spawn_below,
                   DebugCtx* debug_ctx);

void PROJ_handle_projectile_movement(Projectile* proj, bool shoot_upwards);

void PROJ_shoot(Actor* actor, Projectiles* projectiles, DebugCtx* debug_ctx);

#endif
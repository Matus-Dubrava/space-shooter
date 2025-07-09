#include "projectile.h"
#include "actor.h"
#include "core_config.h"
#include "core_debug.h"
#include "core_utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "xp_box.h"

Projectile* PROJ_create_projectile_p(ProjectileInitArgs* args,
                                     DebugCtx* debug_ctx) {
    Projectile* proj = malloc(sizeof(Projectile));
    if (!proj) {
        perror("failed to allocate memory for projectile");
        return NULL;
    }

    debug_ctx->tot_projectiles_spawned++;

    proj->pos = args->pos;
    proj->speed = args->speed;
    proj->down_speed = args->down_speed;
    proj->right_speed = args->right_speed;
    proj->speed_damping = args->speed_damping;
    proj->acceleration = args->acceleration;
    proj->capsule_radius = args->capsule_size;
    proj->damage = args->damage;
    proj->is_valid = true;
    return proj;
}

Projectiles* PROJ_create_projectiles_p(size_t capacity) {
    Projectiles* projectiles = malloc(sizeof(Projectiles));
    if (!projectiles) {
        perror("failed to allocate memory for projectiles");
        return NULL;
    }

    Projectile** items = malloc(capacity * sizeof(Projectile*));
    if (!items) {
        free(projectiles);
        perror("failed to allocate memory for projectiles");
        return NULL;
    }

    projectiles->items = items;
    projectiles->capacity = capacity;
    projectiles->len = 0;
    return projectiles;
}

void PROJ_draw_projectiles(Projectiles* projectiles, Color color) {
    for (size_t i = 0; i < projectiles->len; ++i) {
        if (projectiles->items[i]->is_valid) {
            DrawCircle(projectiles->items[i]->pos.x,
                       projectiles->items[i]->pos.y,
                       projectiles->items[i]->capsule_radius, color);
        }
    }
}

void PROJ_handle_projectile_collision(Actor* actor,
                                      Projectiles* projectiles,
                                      XPBoxes* xp_boxes,
                                      DebugCtx* debug_ctx) {
    // skip the whole thing if the actor is not valid
    if (!actor->is_valid) {
        return;
    }

    for (size_t i = 0; i < projectiles->len; ++i) {
        if (projectiles->items[i]->is_valid) {
            if (CheckCollisionCircles(actor->pos, actor->capsule_radius,
                                      projectiles->items[i]->pos,
                                      projectiles->items[i]->capsule_radius)) {
                // damage the actor that collided with the projectile if iframes
                // are not active
                if (!actor->iframes_active) {
                    // damage shield before damaging health
                    float shield_dmg =
                        minf(actor->shield, projectiles->items[i]->damage);
                    float health_dmg =
                        projectiles->items[i]->damage - shield_dmg;

                    actor->shield -= shield_dmg;
                    actor->health -= health_dmg;

                    // activate iframes if actor can use them
                    if (actor->iframes_enabled) {
                        actor->iframes_active = true;
                        actor->iframes_remainig = actor->n_iframes;
                    }

                    // invalidate actor is its health drowps to 0
                    if (actor->health <= 0) {
                        actor->is_valid = false;

                        if (actor->xp_reward > 0 &&
                            xp_boxes->len < xp_boxes->capacity) {
                            // create new xp box slightly below the actor
                            Vector2 pos = actor->pos;
                            pos.y += actor->capsule_radius + 3;
                            const uint8_t lifetime = DEFAULT_XP_LIFETIME;

                            XPBox* xp_box = XP_create_box_p(
                                pos, (Vector2){.x = 20, .y = 20},
                                actor->xp_reward, lifetime, debug_ctx);
                            xp_boxes->items[xp_boxes->len++] = xp_box;

                        } else if (xp_boxes->len >= xp_boxes->capacity) {
                            debug_ctx->tot_errors++;
                            fprintf(
                                stderr,
                                "can't create more xp boxes, array is full\n");
                        }
                    }
                }

                // invalidate projectile
                projectiles->items[i]->is_valid = false;
            }
        }
    }
}

void PROJ_register(Actor* actor,
                   Projectiles* projectiles,
                   bool spawn_below,
                   DebugCtx* debug_ctx) {
    if (projectiles->len <= projectiles->capacity) {
        ProjectileInitArgs args = {.pos = actor->pos,
                                   .speed = 10,
                                   .capsule_size = 3,
                                   .down_speed = -10,
                                   .right_speed = 0,
                                   .speed_damping = 0,
                                   .acceleration = 0,
                                   .damage = 10};

        Projectile* proj = PROJ_create_projectile_p(&args, debug_ctx);

        // set position of the projectile below or above the actor
        // who launched it
        if (spawn_below) {
            proj->pos.y -= actor->capsule_radius;
        } else {
            proj->pos.y += actor->capsule_radius;
        }

        // register projectile
        projectiles->items[projectiles->len++] = proj;
    } else {
        debug_ctx->tot_errors++;
        fprintf(stderr, "failed to register projectile; array is full\n");
    }
}

int PROJ_clear_invalid_projectiles(Projectiles* projectiles,
                                   DebugCtx* debug_ctx) {
    Projectile** new_items =
        malloc(projectiles->capacity * sizeof(Projectile*));
    if (!new_items) {
        debug_ctx->tot_errors++;
        perror("failed to allocate memory for new projectile items");
        return -1;
    }

    size_t new_projectiles_len = 0;

    for (size_t i = 0; i < projectiles->len; ++i) {
        if (projectiles->items[i]->is_valid) {
            new_items[new_projectiles_len++] = projectiles->items[i];
        } else {
            free(projectiles->items[i]);
        }
    }

    free(projectiles->items);
    projectiles->items = new_items;
    projectiles->len = new_projectiles_len;
    return 0;
}

void PROJ_handle_projectile_movement(Projectile* proj, bool shoot_upwards) {
    if (shoot_upwards) {
        proj->pos.y -= proj->speed;
    } else {
        proj->pos.y += proj->speed;
    }

    if (proj->pos.y > SCREEN_HEIGHT + proj->capsule_radius ||
        proj->pos.y < 0 - proj->capsule_radius) {
        proj->is_valid = false;
    }
}

void PROJ_shoot(Actor* actor, Projectiles* projectiles, DebugCtx* debug_ctx) {
    if (!actor->ongoing_shoot_action_delay) {
        const bool spawn_below = true;
        PROJ_register(actor, projectiles, spawn_below, debug_ctx);
        actor->ongoing_shoot_action_delay = true;
        actor->shoot_action_delay_remaining_frames =
            actor->shoot_action_delay_frames;
    }
}
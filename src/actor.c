#include "actor.h"
#include "core_config.h"
#include "core_debug.h"
#include "raylib.h"

Actors* A_create_actors_p(size_t capacity, DebugCtx* debug_ctx) {
    Actors* actors = malloc(sizeof(Actors));
    if (!actors) {
        perror("failed to allocate memory for actors");
        debug_ctx->tot_errors++;
        return NULL;
    }

    Actor** items = malloc(capacity * sizeof(Actor*));
    if (!items) {
        perror("failed to allocate memory for actors");
        debug_ctx->tot_errors++;
        free(actors);
        return NULL;
    }

    actors->items = items;
    actors->capacity = capacity;
    actors->len = 0;
    return actors;
}

Actor* A_create_test_enemy_p(DebugCtx* debug_ctx) {
    Actor* actor = malloc(sizeof(Actor));
    if (!actor) {
        perror("failed to allocate memory for actor");
        debug_ctx->tot_errors++;
        return NULL;
    }

    int x = GetRandomValue(30, SCREEN_WIDTH - 30);
    int y = GetRandomValue(20, 200);

    actor->pos = (Vector2){.x = x, .y = y};
    actor->speed = 2;
    actor->capsule_radius = 20;
    actor->down_speed = 0;
    actor->right_speed = 0;
    actor->speed_damping = 0.95;
    actor->acceleration = 3;
    actor->shoot_action_delay_frames = 15;
    actor->shoot_action_delay_remaining_frames = 0;
    actor->ongoing_shoot_action_delay = false;
    actor->is_valid = true;
    // actor->health = 10;
    actor->health = 10;
    actor->max_health = 10;
    actor->iframes_active = false;
    actor->iframes_enabled = false;
    actor->iframes_remainig = 0;
    actor->n_iframes = 0;
    actor->health_regen_rate = 0;
    actor->shield = 0;
    actor->shield_regen_rate = 0;
    actor->max_shield = 0;
    actor->level = 0;
    actor->xp = 0;
    actor->xp_reward = 5;
    actor->levelup_xp_required = 0;
    return actor;
}

int A_add_actor(Actors* actors, Actor* actor, DebugCtx* debug_ctx) {
    if (actors->len >= actors->capacity) {
        fprintf(stderr, "failed to add actor, full capacity");
        debug_ctx->tot_errors++;
        return -1;
    }

    debug_ctx->tot_enemies_spawned++;
    actors->items[actors->len++] = actor;
    return 0;
}

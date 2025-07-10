#include "actions.h"
#include "actor.h"
#include "core_debug.h"
#include "projectile.h"

void ACT_shoot(Actor* actor,
               Actors* enemies,
               Projectiles* projectiles,
               DebugCtx* debug_ctx) {
    if (!actor->ongoing_shoot_action_delay) {
        bool spawn_below = true;
        ProjectileInitArgs args = {0};

        if (actor->is_player) {
            PROJ_get_default_guided_args(actor, &args);
            spawn_below = false;
        } else {
            PROJ_get_default_args(actor, &args);
        }

        PROJ_shoot(actor, projectiles, enemies, spawn_below, &args, debug_ctx);

        // this is hardcoded for 2 extra shots for now;
        // TODO: this should be refactored into a function that creates a
        // multishot and handle more than 2 extra shots
        if (actor->multishot_enabled) {
            args.right_speed = 1.5;
            PROJ_shoot(actor, projectiles, enemies, spawn_below, &args,
                       debug_ctx);
            args.right_speed = -1.5;
            PROJ_shoot(actor, projectiles, enemies, spawn_below, &args,
                       debug_ctx);
        }

        // start shooting delay
        actor->ongoing_shoot_action_delay = true;
        actor->shoot_action_delay_remaining_frames =
            actor->shoot_action_delay_frames;
    }
}
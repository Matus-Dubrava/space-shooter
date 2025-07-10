#include "actor.h"
#include "core_config.h"
#include "core_debug.h"
#include "core_ui.h"
#include "core_utils.h"
#include "projectile.h"
#include "raylib.h"
#include "resource_dir.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "xp_box.h"

void handle_movement_action(Actor* actor, MoveActions* move_actions) {
    bool is_updown_key_pressed = false;
    bool is_leftright_key_pressed = false;

    // apply speed if player is holding movement key
    if (move_actions->move_up) {
        actor->down_speed -= actor->speed * actor->acceleration;
        actor->down_speed = maxf(actor->down_speed, -actor->speed);
        is_updown_key_pressed = true;
    }

    if (move_actions->move_down) {
        actor->down_speed += actor->speed * actor->acceleration;
        actor->down_speed = minf(actor->down_speed, actor->speed);
        is_updown_key_pressed = true;
    }

    if (move_actions->move_left) {
        actor->right_speed -= actor->speed * actor->acceleration;
        actor->right_speed = maxf(actor->right_speed, -actor->speed);
        is_leftright_key_pressed = true;
    }

    if (move_actions->move_right) {
        actor->right_speed += actor->speed * actor->acceleration;
        actor->right_speed = minf(actor->right_speed, actor->speed);
        is_leftright_key_pressed = true;
    }

    // apply speed damping if actor is not holding movement key
    if (!is_updown_key_pressed) {
        actor->down_speed *= actor->speed_damping;
    }

    if (!is_leftright_key_pressed) {
        actor->right_speed *= actor->speed_damping;
    }

    // make sure actor can't escape screen boundaries
    if (actor->down_speed > 0 && (actor->pos.y + actor->down_speed +
                                  actor->capsule_radius) < SCREEN_HEIGHT) {
        actor->pos.y += actor->down_speed;
    }

    if (actor->down_speed < 0 &&
        (actor->pos.y + actor->down_speed - actor->capsule_radius) > 0) {
        actor->pos.y += actor->down_speed;
    }

    if (actor->right_speed > 0 && (actor->pos.x + actor->right_speed +
                                   actor->capsule_radius) < SCREEN_WIDTH) {
        actor->pos.x += actor->right_speed;
    }

    if (actor->right_speed < 0 &&
        (actor->pos.x + actor->right_speed - actor->capsule_radius > 0)) {
        actor->pos.x += actor->right_speed;
    }
}

void handle_player_movement(Actor* player) {
    MoveActions move_actions = {0};

    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        move_actions.move_up = true;
    }

    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        move_actions.move_down = true;
    }

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        move_actions.move_left = true;
    }

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        move_actions.move_right = true;
    }

    handle_movement_action(player, &move_actions);
}

void generate_enemy_actions(Actor* enemy,
                            Actors* enemies,
                            Projectiles* projectiles,
                            DebugCtx* debug_ctx) {
    MoveActions move_actions = {0};

    // for now, these are just randomly generated without any real purpose
    // except to have some sort of behaviour on sceen
    if (GetRandomValue(0, 100) < 50) {
        move_actions.move_up = true;
    }

    if (enemy->pos.y < SCREEN_HEIGHT / 2 && (0, 100) < 50) {
        move_actions.move_down = true;
    }

    if (GetRandomValue(0, 100) < 50) {
        move_actions.move_left = true;
    }

    if (GetRandomValue(0, 100) < 50) {
        move_actions.move_right = true;
    }

    if (projectiles->len < projectiles->capacity) {
        if (GetRandomValue(0, 100) < 75) {
            const bool is_guided = false;
            ProjectileInitArgs args = {0};
            PROJ_shoot(enemy, projectiles, enemies, is_guided, &args,
                       debug_ctx);
        }
    }

    handle_movement_action(enemy, &move_actions);
}

void handle_player_actions(Actor* player,
                           Actors* enemies,
                           Projectiles* projectiles,
                           DebugCtx* debug_ctx) {
    if (IsKeyDown(KEY_SPACE) || IsKeyPressed(KEY_SPACE)) {
        const bool is_guided = true;
        const bool is_multishot = true;
        const uint16_t n_proj = 3;
        ProjectileInitArgs args = {0};
        PROJ_shoot(player, projectiles, enemies, is_guided, &args, debug_ctx);
    }

    if (IsKeyPressed(KEY_G)) {
        debug_ctx->is_visible = !debug_ctx->is_visible;
    }
}

void update_actor_timers(Actor* actor) {
    // update shooting timer
    if (actor->ongoing_shoot_action_delay) {
        if (actor->shoot_action_delay_remaining_frames > 0) {
            actor->shoot_action_delay_remaining_frames--;
        } else {
            actor->ongoing_shoot_action_delay = false;
        }
    }

    // update iframes timer
    if (actor->iframes_active) {
        actor->iframes_remainig--;
        if (actor->iframes_remainig <= 0) {
            actor->iframes_active = false;
        }
    }
}

void regenerate_shield_and_health(Actor* actor) {
    // regenerate both shield and health
    if (actor->health_regen_rate > 0) {
        actor->health += actor->health_regen_rate / TARGET_FPS;
        actor->health = minf(actor->health, actor->max_health);
    }

    if (actor->shield_regen_rate > 0) {
        actor->shield += actor->shield_regen_rate / TARGET_FPS;
        actor->shield = minf(actor->shield, actor->max_shield);
    }
}

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Shooter");
    SearchAndSetResourceDir("resources");
    SetTargetFPS(TARGET_FPS);

    DebugCtx debug_ctx;
    DBG_init(&debug_ctx);

    // init player
    size_t player_capsule_radius = 20;
    Actor player = {
        .pos = (Vector2){.x = SCREEN_WIDTH / 2 - (player_capsule_radius),
                         .y = SCREEN_HEIGHT - 100},
        .speed = 4,
        .capsule_radius = player_capsule_radius,
        .down_speed = 0,
        .right_speed = 0,
        .speed_damping = 0.95,
        .acceleration = .35,
        .shoot_action_delay_frames = 15,
        .shoot_action_delay_remaining_frames = 0,
        .ongoing_shoot_action_delay = false,
        .is_valid = true,
        .health = 100,
        .max_health = 100,
        .health_regen_rate = 0,
        .shield = 100,
        .max_shield = 100,
        .shield_regen_rate = 2,
        .iframes_enabled = true,
        .iframes_active = false,
        .n_iframes = 30,
        .iframes_remainig = 0,
        .level = 0,
        .xp = 0,
        .xp_reward = 0,
        .levelup_xp_required = 10};

    // init enemies
    size_t n_enemies = 3;
    Actors* enemies = A_create_actors_p(1024, &debug_ctx);
    if (!enemies) {
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < n_enemies; ++i) {
        Actor* enemy = A_create_test_enemy_p(&debug_ctx);
        if (enemy) {
            A_add_actor(enemies, enemy, &debug_ctx);
        }
    }

    // init player projectiles
    Projectiles* player_projectiles =
        PROJ_create_projectiles_p(MAX_PLAYER_PROJECTILES);
    if (!player_projectiles) {
        exit(EXIT_FAILURE);
    }

    Projectiles* enemy_projectiles =
        PROJ_create_projectiles_p(MAX_ENEMY_PROJECTILES);
    if (!enemy_projectiles) {
        exit(EXIT_FAILURE);
    }

    // init xp boxes
    XPBoxes* xp_boxes = XP_create_boxes_p(MAX_XP_BOXES);
    if (!xp_boxes) {
        exit(EXIT_FAILURE);
    }

    while (!WindowShouldClose()) {
        // player
        update_actor_timers(&player);
        handle_player_movement(&player);
        handle_player_actions(&player, enemies, player_projectiles, &debug_ctx);
        regenerate_shield_and_health(&player);

        // player projectiles
        PROJ_update_projectiles_timers(player_projectiles);
        for (size_t i = 0; i < player_projectiles->len; ++i) {
            if (player_projectiles->items[i]->is_valid) {
                const bool shoot_upwards = true;
                PROJ_handle_projectile_movement(player_projectiles->items[i],
                                                shoot_upwards);
            }
        }

        // enemies
        for (size_t i = 0; i < enemies->len; ++i) {
            if (enemies->items[i]->is_valid) {
                update_actor_timers(enemies->items[i]);
                generate_enemy_actions(enemies->items[i], enemies,
                                       enemy_projectiles, &debug_ctx);
                regenerate_shield_and_health(enemies->items[i]);
            }
        }

        // enemy projectiles
        for (size_t i = 0; i < enemy_projectiles->len; ++i) {
            if (enemy_projectiles->items[i]->is_valid) {
                bool shoot_upwards = false;
                PROJ_update_projectiles_timers(enemy_projectiles);
                PROJ_handle_projectile_movement(enemy_projectiles->items[i],
                                                shoot_upwards);
            }
        }

        // xp boxes
        for (size_t i = 0; i < xp_boxes->len; ++i) {
            if (xp_boxes->items[i]->is_valid) {
                XP_move_box(xp_boxes->items[i]);
                XP_update_lifetime_timer(xp_boxes->items[i]);
            }
        }

        // player collision with enemy bullets; skip if iframes are active
        PROJ_handle_projectile_collision(&player, enemy_projectiles, xp_boxes,
                                         &debug_ctx);

        // player collision with xp boxes
        for (size_t i = 0; i < xp_boxes->len; ++i) {
            XP_handle_collision_with_player(&player, xp_boxes->items[i],
                                            &debug_ctx);
        }

        // enemy collision with player bullets
        for (size_t i = 0; i < n_enemies; ++i) {
            PROJ_handle_projectile_collision(
                enemies->items[i], player_projectiles, xp_boxes, &debug_ctx);
        }

        // clear pojectiles
        if (PROJ_clear_invalid_projectiles(player_projectiles, &debug_ctx) !=
            0) {
            fprintf(stderr, "failed to clear player projectiles\n");
        };
        if (PROJ_clear_invalid_projectiles(enemy_projectiles, &debug_ctx) !=
            0) {
            fprintf(stderr, "failed to clear enemy projectiles\n");
        }

        // draw
        BeginDrawing();
        if (player.health <= 0) {
            draw_game_over_screen();
        } else {
            ClearBackground(BLACK);
            DrawCircle(player.pos.x, player.pos.y, player.capsule_radius,
                       BLUE);  // player

            // draw enemies
            for (size_t i = 0; i < enemies->len; ++i) {
                if (enemies->items[i]->is_valid) {
                    DrawCircle(enemies->items[i]->pos.x,
                               enemies->items[i]->pos.y,
                               enemies->items[i]->capsule_radius, YELLOW);
                }
            }

            XP_draw_boxes(xp_boxes);
            PROJ_draw_projectiles(player_projectiles, PURPLE);
            PROJ_draw_projectiles(enemy_projectiles, RED);
            draw_HUD(&player);
        }
#ifdef DEBUG
        if (debug_ctx.is_visible) {
            DBG_draw(&debug_ctx);
        }
#endif
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

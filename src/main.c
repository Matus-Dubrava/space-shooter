#include "actor.h"
#include "core_config.h"
#include "core_ui.h"
#include "core_utils.h"
#include "raylib.h"
#include "resource_dir.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

// move_actions: array of movement actions
// [up, down, left, right]
void handle_movement_action(Actor* actor, bool* move_actions) {
    bool is_updown_key_pressed = false;
    bool is_leftright_key_pressed = false;

    // apply speed if player is holding movement key
    if (move_actions[0]) {
        actor->down_speed -= actor->speed * actor->acceleration;
        actor->down_speed = maxf(actor->down_speed, -actor->speed);
        is_updown_key_pressed = true;
    }

    if (move_actions[1]) {
        actor->down_speed += actor->speed * actor->acceleration;
        actor->down_speed = minf(actor->down_speed, actor->speed);
        is_updown_key_pressed = true;
    }

    if (move_actions[2]) {
        actor->right_speed -= actor->speed * actor->acceleration;
        actor->right_speed = maxf(actor->right_speed, -actor->speed);
        is_leftright_key_pressed = true;
    }

    if (move_actions[3]) {
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
    bool move_actions[4] = {false};

    // apply speed if player is holding movement key
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        move_actions[0] = true;
    }

    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        move_actions[1] = true;
    }

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        move_actions[2] = true;
    }

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        move_actions[3] = true;
    }

    handle_movement_action(player, move_actions);
}

void generate_enemy_actions(Actor* enemy,
                            Projectile** projectiles,
                            size_t* n_projectiles,
                            size_t max_projecties) {
    bool move_actions[4] = {false};
    bool shoot = false;

    if (GetRandomValue(0, 100) < 50) {
        move_actions[0] = true;
    }

    if (GetRandomValue(0, 100) < 50) {
        move_actions[1] = true;
    }

    if (GetRandomValue(0, 100) < 50) {
        move_actions[2] = true;
    }

    if (GetRandomValue(0, 100) < 50) {
        move_actions[3] = true;
    }

    if (GetRandomValue(0, 100) < 50) {
        shoot = true;
    }

    if (*n_projectiles < max_projecties) {
        if (GetRandomValue(0, 100) < 75) {
            add_projectile(enemy, projectiles, n_projectiles, max_projecties,
                           false);
        }
    }

    handle_movement_action(enemy, move_actions);
}

void handle_projectile_movement(Projectile* proj, bool shoot_upwards) {
    // handles projectile movement
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

void add_projectile(Actor* actor,
                    Projectile** projectiles,
                    size_t* n_projectiles,
                    size_t max_projectiles,
                    bool spawn_below) {
    if (*n_projectiles <= max_projectiles) {
        Projectile* proj =
            create_projectile(actor->pos, 10, 3, -10, 0, 0, 0, 10, true);

        // set position of the projectile below or above the actor
        // who launched it
        if (spawn_below) {
            proj->pos.y -= actor->capsule_radius;
        } else {
            proj->pos.y += actor->capsule_radius;
        }

        // register projectile
        projectiles[*n_projectiles] = proj;
        *n_projectiles += 1;
    }
}

void handle_player_shooting(Actor* player,
                            Actor** projectiles,
                            size_t* n_projectiles,
                            size_t max_projectiles) {
    if (IsKeyDown(KEY_SPACE) || IsKeyPressed(KEY_SPACE)) {
        if (!player->ongoing_shoot_action_delay) {
            add_projectile(player, projectiles, n_projectiles, max_projectiles,
                           true);
            player->ongoing_shoot_action_delay = true;
            player->shoot_action_delay_remaining_frames =
                player->shoot_action_delay_frames;
        }
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

void handle_projectile_collision(Actor* actor,
                                 Projectile** projectiles,
                                 size_t n_projectiles) {
    for (size_t i = 0; i < n_projectiles; ++i) {
        if (projectiles[i]->is_valid) {
            if (CheckCollisionCircles(actor->pos, actor->capsule_radius,
                                      projectiles[i]->pos,
                                      projectiles[i]->capsule_radius)) {
                // damage the actor that collided with the projectile if iframes
                // are not active
                if (!actor->iframes_active) {
                    actor->health -= projectiles[i]->damage;

                    // activate iframes if actor can use them
                    if (actor->iframes_enabled) {
                        actor->iframes_active = true;
                        actor->iframes_remainig = actor->n_iframes;
                    }

                    // invalidate actor is its health drowps to 0
                    if (actor->health <= 0) {
                        actor->is_valid = false;
                    }
                }

                // invalidate projectile
                projectiles[i]->is_valid = false;
            }
        }
    }
}

// create new array of projectiles with still valid projectiles;
// frees up old projectiles
Projectile** clear_invalid_projectiles(Projectile** projectiles,
                                       size_t* n_projectiles,
                                       size_t max_projectiles) {
    Projectile** new_projectiles =
        malloc(max_projectiles * sizeof(Projectile*));
    if (!new_projectiles) {
        perror("failed to allocate memory for projectiles");
        return;
    }
    size_t n_new_projectiles = 0;

    for (size_t i = 0; i < max_projectiles; ++i) {
        if (projectiles[i]->is_valid) {
            new_projectiles[n_new_projectiles++] = projectiles[i];
        } else {
            free(projectiles[i]);
        }
    }

    *n_projectiles = n_new_projectiles;
    free(projectiles);
    return new_projectiles;
}

void draw_projectiles(Projectile** projectiles,
                      size_t n_projectiles,
                      Color color) {
    for (size_t i = 0; i < n_projectiles; ++i) {
        if (projectiles[i]->is_valid) {
            DrawCircle(projectiles[i]->pos.x, projectiles[i]->pos.y,
                       projectiles[i]->capsule_radius, color);
        }
    }
}

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Shooter");
    SearchAndSetResourceDir("resources");
    SetTargetFPS(TARGET_FPS);

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
        .iframes_enabled = true,
        .iframes_active = false,
        .n_iframes = 30,
        .iframes_remainig = 0};

    // init enemies
    size_t n_enemies = 2;
    Actor enemies[n_enemies];
    for (size_t i = 0; i < n_enemies; ++i) {
        int x = GetRandomValue(30, SCREEN_WIDTH - 30);
        int y = GetRandomValue(20, 200);
        enemies[i] = (Actor){.pos = (Vector2){.x = x, .y = y},
                             .speed = 4,
                             .capsule_radius = 20,
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
                             .iframes_active = false,
                             .iframes_enabled = false,
                             .iframes_remainig = 0,
                             .n_iframes = 0};
    }

    // init player projectiles
    size_t max_player_projectiles = 1024;
    size_t n_player_projectiles = 0;
    Projectile** player_projectiles =
        malloc(max_player_projectiles * sizeof(Projectile*));
    if (!player_projectiles) {
        perror("failed to allocate memory for player projectiles");
        exit(EXIT_FAILURE);
    }

    // init enemy projecties
    size_t max_enemy_projecties = 1024;
    size_t n_enemy_projectiles = 0;
    Projectile** enemy_projectiles =
        malloc(max_enemy_projecties * sizeof(Projectile*));
    if (!enemy_projectiles) {
        perror("failed to allocate memory for enemy projectiles");
        exit(EXIT_FAILURE);
    }

    // size_t frame = 0;

    while (!WindowShouldClose()) {
        // frame++;
        // if (frame % TARGET_FPS == 0) {
        //     player.health--;
        // }

        // player
        update_actor_timers(&player);
        handle_player_movement(&player);
        handle_player_shooting(&player, player_projectiles,
                               &n_player_projectiles, max_player_projectiles);

        for (size_t i = 0; i < n_player_projectiles; ++i) {
            if (player_projectiles[i]->is_valid) {
                handle_projectile_movement(player_projectiles[i], true);
            }
        }

        // enemies
        for (size_t i = 0; i < n_enemies; ++i) {
            if (enemies[i].is_valid) {
                generate_enemy_actions(&enemies[i], enemy_projectiles,
                                       &n_enemy_projectiles,
                                       max_enemy_projecties);
            }
        }

        for (size_t i = 0; i < n_enemy_projectiles; ++i) {
            if (enemy_projectiles[i]->is_valid) {
                handle_projectile_movement(enemy_projectiles[i], false);
            }
        }

        // player collision with enemy bullets; skip if iframes are active
        handle_projectile_collision(&player, enemy_projectiles,
                                    n_enemy_projectiles);

        // enemy collision with player bullets
        for (size_t i = 0; i < n_enemies; ++i) {
            handle_projectile_collision(&enemies[i], player_projectiles,
                                        n_player_projectiles);
        }

        // clear pojectiles
        if (n_enemy_projectiles >= max_enemy_projecties) {
            enemy_projectiles = clear_invalid_projectiles(
                enemy_projectiles, &n_enemy_projectiles, max_enemy_projecties);
        }

        if (n_player_projectiles >= max_player_projectiles) {
            player_projectiles = clear_invalid_projectiles(
                player_projectiles, &n_player_projectiles,
                max_player_projectiles);
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
            for (size_t i = 0; i < n_enemies; ++i) {
                if (enemies[i].is_valid) {
                    DrawCircle(enemies[i].pos.x, enemies[i].pos.y,
                               enemies[i].capsule_radius, YELLOW);
                }
            }

            draw_projectiles(player_projectiles, n_player_projectiles, PURPLE);
            draw_projectiles(enemy_projectiles, n_enemy_projectiles, RED);
            draw_HUD(&player);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

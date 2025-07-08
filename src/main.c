#include "actor.h"
#include "core_config.h"
#include "core_ui.h"
#include "core_utils.h"
#include "raylib.h"
#include "resource_dir.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void handle_player_movement(Actor* player) {
    bool is_updown_key_pressed = false;
    bool is_leftright_key_pressed = false;

    // apply speed if player is holding movement key
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        player->down_speed -= player->speed * player->acceleration;
        player->down_speed = maxf(player->down_speed, -player->speed);
        is_updown_key_pressed = true;
    }

    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        player->down_speed += player->speed * player->acceleration;
        player->down_speed = minf(player->down_speed, player->speed);
        is_updown_key_pressed = true;
    }

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player->right_speed -= player->speed * player->acceleration;
        player->right_speed = maxf(player->right_speed, -player->speed);
        is_leftright_key_pressed = true;
    }

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player->right_speed += player->speed * player->acceleration;
        player->right_speed = minf(player->right_speed, player->speed);
        is_leftright_key_pressed = true;
    }

    // apply speed damping if player is not holding movement key
    if (!is_updown_key_pressed) {
        player->down_speed *= player->speed_damping;
    }

    if (!is_leftright_key_pressed) {
        player->right_speed *= player->speed_damping;
    }

    // make sure player can't escape screen boundaries
    if (player->down_speed > 0 && (player->pos.y + player->down_speed +
                                   player->capsule_radius) < SCREEN_HEIGHT) {
        player->pos.y += player->down_speed;
    }

    if (player->down_speed < 0 &&
        (player->pos.y + player->down_speed - player->capsule_radius) > 0) {
        player->pos.y += player->down_speed;
    }

    if (player->right_speed > 0 && (player->pos.x + player->right_speed +
                                    player->capsule_radius) < SCREEN_WIDTH) {
        player->pos.x += player->right_speed;
    }

    if (player->right_speed < 0 &&
        (player->pos.x + player->right_speed - player->capsule_radius > 0)) {
        player->pos.x += player->right_speed;
    }
}

void handle_projectile_movement(Actor* proj, bool shoot_upwards) {
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

void add_projectile(Actor* player,
                    Actor** projectiles,
                    size_t* n_projectiles,
                    size_t max_projectiles) {
    if (*n_projectiles <= max_projectiles) {
        Actor* proj = malloc(sizeof(Actor));
        if (!proj) {
            perror("failed to allocate memory for player projectile");
            return NULL;
        }
        memset(proj, 0, sizeof(proj));

        proj->acceleration = 1;
        proj->capsule_radius = 3;
        proj->down_speed = -10;
        proj->right_speed = 0;
        proj->pos = player->pos;
        proj->pos.y -= player->capsule_radius;
        proj->speed = 10;
        proj->speed_damping = 0;
        proj->is_valid = true;

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
            add_projectile(player, projectiles, n_projectiles, max_projectiles);
            player->ongoing_shoot_action_delay = true;
            player->shoot_action_delay_remaining_frames =
                player->shoot_action_delay_frames;
        }
    }
}

void update_player_timers(Actor* player) {
    // update shooting timer
    if (player->ongoing_shoot_action_delay) {
        if (player->shoot_action_delay_remaining_frames > 0) {
            player->shoot_action_delay_remaining_frames--;
        } else {
            player->ongoing_shoot_action_delay = false;
        }
    }
}

void draw_projectiles(Actor** projectiles, size_t n_projectiles) {
    for (size_t i = 0; i < n_projectiles; ++i) {
        DrawCircle(projectiles[i]->pos.x, projectiles[i]->pos.y,
                   projectiles[i]->capsule_radius, RED);
    }
}

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Shooter");
    SearchAndSetResourceDir("resources");
    SetTargetFPS(TARGET_FPS);

    Actor player = {.pos = (Vector2){.x = 200, .y = 200},
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
                    .max_health = 100};

    size_t max_player_projectiles = 1024;
    size_t n_player_projectiles = 0;
    Actor** player_projectiles =
        malloc(max_player_projectiles * sizeof(Actor*));
    if (!player_projectiles) {
        perror("failed to allocate memory for player bullets");
        exit(EXIT_FAILURE);
    }

    // size_t frame = 0;

    while (!WindowShouldClose()) {
        // frame++;
        // if (frame % TARGET_FPS == 0) {
        //     player.health--;
        // }

        update_player_timers(&player);
        handle_player_movement(&player);
        handle_player_shooting(&player, player_projectiles,
                               &n_player_projectiles, max_player_projectiles);

        for (size_t i = 0; i < n_player_projectiles; ++i) {
            if (player_projectiles[i]->is_valid) {
                handle_projectile_movement(player_projectiles[i], true);
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawCircle(player.pos.x, player.pos.y, player.capsule_radius, BLUE);
        draw_projectiles(player_projectiles, n_player_projectiles);
        draw_HUD(&player);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

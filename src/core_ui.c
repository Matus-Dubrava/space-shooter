#include "core_ui.h"
#include "actor.h"
#include "core_config.h"

void draw_healthbar(Actor* player) {
    DrawRectangle(SCREEN_WIDTH / 2 - (player->max_health / 2),
                  SCREEN_HEIGHT - 30, player->max_health, 20, GRAY);

    DrawRectangle(SCREEN_WIDTH / 2 - (player->health / 2), SCREEN_HEIGHT - 30,
                  player->health, 20, RED);
}

void draw_HUD(Actor* player) {
    draw_healthbar(player);
}

void draw_game_over_screen() {
    ClearBackground(BLACK);
    // TODO: this need to be properly centered, accounting for acutal text size
    DrawText("GAME OVER", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, 40, RED);
}
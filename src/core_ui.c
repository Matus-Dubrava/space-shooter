#include "core_ui.h"
#include "actor.h"
#include "core_config.h"

void draw_healthbar(Actor* player) {
    int bar_height = 15;

    DrawRectangle(SCREEN_WIDTH / 2 - (player->max_health / 2),
                  SCREEN_HEIGHT - 30, player->max_health, bar_height, GRAY);

    DrawRectangle(SCREEN_WIDTH / 2 - (player->health / 2), SCREEN_HEIGHT - 30,
                  player->health, bar_height, RED);
}

void draw_shieldbar(Actor* player) {
    int bar_height = 15;

    DrawRectangle(SCREEN_WIDTH / 2 - (player->max_shield / 2),
                  SCREEN_HEIGHT - 60, player->max_shield, bar_height, GRAY);

    DrawRectangle(SCREEN_WIDTH / 2 - (player->shield / 2), SCREEN_HEIGHT - 60,
                  player->shield, bar_height, BLUE);
}

void draw_HUD(Actor* actor) {
    draw_healthbar(actor);
    draw_shieldbar(actor);
}

void draw_game_over_screen() {
    ClearBackground(BLACK);
    // TODO: this need to be properly centered, accounting for acutal text size
    DrawText("GAME OVER", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, 40, RED);
}
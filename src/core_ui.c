#include "core_ui.h"
#include "actor.h"
#include "core_config.h"
#include "stdio.h"

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

void draw_xp_info(Actor* player) {
    char xp_info_buf[100];
    sprintf(xp_info_buf, "XP: %zu", player->xp);
    char level_info_buf[100];
    sprintf(level_info_buf, "LVL: %d", player->level);

    const int x = 20;
    const int y = SCREEN_HEIGHT - 60;
    const int font_size = 24;
    const Color color = WHITE;
    DrawText(xp_info_buf, x, y, font_size, color);
    DrawText(level_info_buf, x, y + 30, font_size, color);
}

void draw_HUD(Actor* actor) {
    draw_healthbar(actor);
    draw_shieldbar(actor);
    draw_xp_info(actor);
}

void draw_game_over_screen() {
    ClearBackground(BLACK);
    // TODO: this need to be properly centered, accounting for acutal text size
    DrawText("GAME OVER", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, 40, RED);
}
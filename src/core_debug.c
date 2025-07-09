#include "core_debug.h"

#include "raylib.h"
#include "stdbool.h"
#include "stdio.h"

void DBG_init(DebugCtx* ctx) {
    ctx->enemies_projectiles_clear_calls = 0;
    ctx->player_projectiles_clear_calls = 0;
    ctx->tot_enemies_spawned = 0;
    ctx->tot_projectiles_spawned = 0;
    ctx->tot_xpboxes_spawned = 0;
    ctx->tot_xpboxes_pickedup = 0;
    ctx->tot_errors = 0;
    ctx->tot_warnings = 0;
    ctx->is_visible = false;
}

void DBG_draw(DebugCtx* debug_ctx) {
    char buf[100];
    sprintf(buf, "enemy proj. clear calls: %zu",
            debug_ctx->enemies_projectiles_clear_calls);

    int x = 10;
    int y = 10;
    int font_size = 18;
    Color color = GREEN;
    DrawText(buf, x, y, font_size, color);

    sprintf(buf, "player proj. clear calls: %zu",
            debug_ctx->player_projectiles_clear_calls);
    y += 15;
    DrawText(buf, x, y, font_size, color);

    sprintf(buf, "tot enemies spawned: %zu", debug_ctx->tot_enemies_spawned);
    y += 15;
    DrawText(buf, x, y, font_size, color);

    sprintf(buf, "tot proj. spawned: %zu", debug_ctx->tot_projectiles_spawned);
    y += 15;
    DrawText(buf, x, y, font_size, color);

    sprintf(buf, "tot xpbox spawned/picked: %zu/%zu",
            debug_ctx->tot_xpboxes_spawned, debug_ctx->tot_xpboxes_pickedup);
    y += 15;
    DrawText(buf, x, y, font_size, color);

    sprintf(buf, "tot errors: %zu", debug_ctx->tot_errors);
    y += 15;
    DrawText(buf, x, y, font_size, RED);

    sprintf(buf, "tot warnings: %zu", debug_ctx->tot_warnings);
    y += 15;
    DrawText(buf, x, y, font_size, YELLOW);
}
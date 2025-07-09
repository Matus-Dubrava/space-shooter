#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#include "stdbool.h"
#include "stdlib.h"

typedef struct DebugCtx {
    size_t tot_projectiles_spawned;
    size_t player_projectiles_clear_calls;
    size_t enemies_projectiles_clear_calls;
    size_t tot_enemies_spawned;
    size_t tot_xpboxes_spawned;
    size_t tot_xpboxes_pickedup;
    size_t tot_errors;
    size_t tot_warnings;
    bool is_visible;
} DebugCtx;

void DBG_init(DebugCtx* ctx);

void DBG_draw(DebugCtx* ctx);

#endif
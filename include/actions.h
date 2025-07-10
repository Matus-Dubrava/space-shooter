#ifndef ACTIONS_H
#define ACTIONS_H

typedef struct Actor Actor;
typedef struct Actors Actors;
typedef struct DebugCtx DebugCtx;
typedef struct Projectiles Projectiles;

typedef enum Actions { SHOOT } Actions;

void ACT_shoot(Actor* actor,
               Actors* enemies,
               Projectiles* projectiles,
               DebugCtx* debug_ctx);

#endif
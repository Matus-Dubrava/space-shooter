#include "actor.h"

Projectile* create_projectile(Vector2 pos,
                              float speed,
                              float capsule_size,
                              float down_speed,
                              float right_speed,
                              float speed_damping,
                              float acceleration,
                              float damage,
                              bool is_valid) {
    Projectile* proj = malloc(sizeof(Projectile));
    if (!proj) {
        perror("failed to allocate memory for projectile");
        return NULL;
    }

    proj->pos = pos;
    proj->speed = speed;
    proj->down_speed = down_speed;
    proj->right_speed = right_speed;
    proj->speed_damping = speed_damping;
    proj->acceleration = acceleration;
    proj->capsule_radius = capsule_size;
    proj->damage = damage;
    proj->is_valid = is_valid;
    return proj;
}
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "entity.h"
#include "kmath.h"

void EntInit(Entity *ent, uint8_t type) {
	
}

// Move entity by it's velocity scaled by delta time
void EntUpdatePosition(Entity *ent, float dt) {
	ent->position = Vector2Add(ent->position, Vector2Scale(ent->velocity, dt));	
}

void EntSpin(Entity *ent, float dt) {
	ent->angle += ent->angle_vel * dt;
	if(ent->angle > 360 * DEG2RAD) ent->angle = 0 * DEG2RAD;
	else if(ent->angle < 0 * DEG2RAD) ent->angle = 360 * DEG2RAD;

	ent->sprite_angle = ent->angle * RAD2DEG;
}

Vector2 EntCenter(Entity *ent) {
	return Vector2Add(ent->position, ent->center_offset);
}

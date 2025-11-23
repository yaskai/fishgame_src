#include "raylib.h"
#include "entity.h"
#include "sprites.h"

void AsteroidUpdate(Entity *asteroid, float dt) {
	EntUpdatePosition(asteroid, dt);
	EntSpin(asteroid, dt);
}

void AsteroidDraw(Entity *asteroid, SpriteLoader *sl) {
	DrawSpritePro(&sl->spr_pool[1], 0, asteroid->position, asteroid->sprite_angle, asteroid->scale, 0);
	//DrawCircleV(EntCenter(asteroid), asteroid->radius, RED);
}

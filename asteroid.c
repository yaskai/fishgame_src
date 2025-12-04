#include "raylib.h"

#include "entity.h"
#include "sprites.h"

void AsteroidUpdate(Entity *asteroid, float dt) {
	EntUpdatePosition(asteroid, dt);
	EntSpin(asteroid, dt);
}

void AsteroidDraw(Entity *asteroid, SpriteLoader *sl) {
	AsteroidData *ast_data = asteroid->data;
	DrawSpritePro(&sl->spr_pool[SHEET_ASTEROIDS], ast_data->frame, asteroid->position, asteroid->sprite_angle, asteroid->scale, 0);
}

#include <stdio.h>
#include "raylib.h"
#include "entity.h"
#include "sprites.h"
#include "raymath.h"

void FishUpdate(Entity *fish, float dt) {
	FishData *f = fish->data;	

	EntUpdatePosition(fish, dt);

	if(f->state == FISH_CAUGHT) return;

	f->timer -= dt;
	if(f->timer < 0) {
		f->state++;
		if(f->state == FISH_CAUGHT) f->state = FISH_IDLE; 
		f->timer = GetRandomValue(0, 3);

		if(f->state == FISH_SWIM) {
			Vector2 dir = (Vector2){GetRandomValue(-10, 10) * 0.01f, GetRandomValue(-10, 10) * 0.01f};
			f->dir = dir;
		} 
	}

	if(f->state == FISH_SWIM)
		fish->velocity = Vector2Add(fish->velocity, Vector2Scale(f->dir, 1000 * dt));
	else if(f->state == FISH_IDLE)
		fish->velocity = Vector2Lerp(fish->velocity, Vector2Zero(), dt);
}

void FishDraw(Entity *fish, SpriteLoader *sl) {
	FishData *f = fish->data;	
	//DrawSpritePro(&sl->spr_pool[2], 0, fish->position, fish->sprite_angle, fish->scale, 0);
	//AnimDraw(&sl->anims[1], fish->position, 0);
 
	uint8_t draw_flags = 0;
	if(f->dir.x > 0) draw_flags |= SPR_FLIP_X;

	if(f->state == 1)
		AnimDraw(&sl->anims[1], fish->position, draw_flags);
	else 
		DrawSpritePro(&sl->spr_pool[2], 0, fish->position, fish->sprite_angle, fish->scale, draw_flags);

	DrawText(TextFormat("%d", fish->id), fish->position.x, fish->position.y, 32, GREEN);
}


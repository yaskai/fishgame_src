#include <stdio.h>
#include "raylib.h"
#include "entity.h"
#include "sprites.h"
#include "raymath.h"

void FishUpdate(Entity *fish, float dt) {
	FishData *fish_data = fish->data;	

	EntUpdatePosition(fish, dt);

	if(fish_data->state == FISH_CAUGHT) return;

	fish_data->timer -= dt;

	if(fish_data->timer < 0) {

		fish_data->state++;

		if(fish_data->state == FISH_CAUGHT) fish_data->state = FISH_IDLE; 

		fish_data->timer = GetRandomValue(0, 3);

		if(fish_data->state == FISH_SWIM) {
			Vector2 dir = (Vector2){GetRandomValue(-10, 10) * 0.01f, GetRandomValue(-10, 10) * 0.01f};
			fish_data->dir = dir;
		} 
	}

	if(fish_data->state == FISH_SWIM)
		fish->velocity = Vector2Add(fish->velocity, Vector2Scale(fish_data->dir, 1000 * dt));
	else if(fish_data->state == FISH_IDLE)
		fish->velocity = Vector2Lerp(fish->velocity, Vector2Zero(), dt);
}

void FishDraw(Entity *fish, SpriteLoader *sl) {
	FishData *fish_data = fish->data;	
 
	uint8_t draw_flags = 0;
	if(fish_data->dir.x > 0) draw_flags |= SPR_FLIP_X;

	Spritesheet *sheet = &sl->spr_pool[SHEET_FISH_00 + fish_data->subtype];
	SpriteAnimation *anim = &sl->anims[ANIM_FISH_SWIM_00 + fish_data->subtype];

	if(fish_data->state == 1)
		AnimDraw(anim, fish->position, draw_flags);
	else 
		DrawSpritePro(sheet, 0, fish->position, fish->sprite_angle, fish->scale, draw_flags);

	//DrawText(TextFormat("%d", fish->id), fish->position.x, fish->position.y, 32, GREEN);
}


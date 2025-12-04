#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"

#include "sprites.h"
#include "bg.h"

void BgInit(Background *bg, SpriteLoader *sl) {
	bg->sl = sl;

	/*
	bg->src_rec = (Rectangle) {
		.x = 0,
		.y = 0,
		.width = 1920,
		.height = 1920
	};

	bg->dst_rec = (Rectangle) {
		.x = 1920 * 0.5f,
		.y = 1920 * 0.5f,
		.width = 1920,
		.height = 1920
	};
	*/

	bg->src_rec = (Rectangle) {
		.x = 0,
		.y = 0,
		.width = 1920  * 4,
		//.height = 1080 * 4
		.height = 1920 * 4
	};

	bg->dst_rec = (Rectangle) {
		.x = 1920 * 0.5f,
		//.y = 1080 * 0.5f,
		.y = 1920 * 0.5f,
		.width = 1920,
		//.height = 1080
		.height = 1920
	};
		
	bg->render_texture = LoadRenderTexture(bg->src_rec.width, bg->src_rec.height);
	
	for(uint8_t i = 0; i < LAYER_COUNT; i++) {
		bg->layers[i].stars = calloc(STAR_COUNT, sizeof(Star));
		bg->layers[i].rt = LoadRenderTexture(1920, 1920);
		SetTextureWrap(bg->layers[i].rt.texture, TEXTURE_WRAP_REPEAT);

		for(uint16_t j = 0; j < STAR_COUNT; j++) {
			Star *star = &bg->layers[i].stars[j];

			star->position = (Vector2){ GetRandomValue(0, bg->src_rec.width), GetRandomValue(0, bg->src_rec.height)};
			star->scale = GetRandomValue(1, 5) * 0.1f;

			star->anim_id = GetRandomValue(ANIM_STAR_00, ANIM_STAR_03);
			star->frame_offset = GetRandomValue(0, bg->sl->anims[star->anim_id].frame_count - 1);

			star->sprite_id = bg->sl->anims[star->anim_id].spritesheet->id;

			star->color_id = GetRandomValue(0, 4);
		}

		bg->layers[i].scroll_mod = 0.1f + ((i+1) * 0.1f);
	}
}

void BgUpdate(Background *bg, float dt) {
	for(uint8_t i = ANIM_STAR_00; i < ANIM_STAR_03; i++) {
		AnimPlay(&bg->sl->anims[i], dt);
	}
}

void BgDraw(Background *bg) {
	Color colors[] = { WHITE, PINK, SKYBLUE, ORANGE };

	for(uint8_t i = 0; i < LAYER_COUNT; i++) {
		BeginTextureMode(bg->layers[i].rt);
		ClearBackground((Color){0});

		for(uint16_t j = 0; j < STAR_COUNT; j++) {
			Star *star = &bg->layers[i].stars[j];

			uint16_t frame = 
				(bg->sl->anims[star->anim_id].cur_frame + star->frame_offset) % bg->sl->anims[star->anim_id].frame_count;

			DrawSpriteRecolor(
				&bg->sl->spr_pool[star->sprite_id],
				frame,
				star->position,
				0,
				star->scale,
				0,
				colors[star->color_id]
			);
		}

		EndTextureMode();
	};
}


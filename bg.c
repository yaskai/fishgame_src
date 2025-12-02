#include <stdint.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "sprites.h"
#include "bg.h"

void BgInit(Background *bg, SpriteLoader *sl) {
	bg->sl = sl;

	bg->src_rec = (Rectangle) {
		.x = 0,
		.y = 0,
		.width = 1920,
		.height = 1080
	};
		
	bg->render_texture = LoadRenderTexture(bg->src_rec.width, bg->src_rec.height);
	
	for(uint8_t i = 0; i < LAYER_COUNT; i++) {
		Star stars[STAR_COUNT] = {0};

		for(uint8_t j = 0; j < STAR_COUNT; j++) {
			stars[j].position = (Vector2){ GetRandomValue(0, bg->src_rec.width), GetRandomValue(0, bg->src_rec.height)};
			stars[j].scale = GetRandomValue(5, 20) * 0.1f;

			stars[j].anim_id = GetRandomValue(ANIM_STAR_00, ANIM_STAR_03);
			stars[j].frame_offset = GetRandomValue(0, bg->sl->anims[stars[j].anim_id].frame_count - 1);

			stars[j].sprite_id = bg->sl->anims[stars[j].anim_id].spritesheet->id;

			stars[j].color_id = GetRandomValue(0, 4);
		}

		memcpy(bg->layers[i].stars, stars, sizeof(stars));

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

	//BeginTextureMode(bg->render_texture);
	//ClearBackground((Color){0});

	for(uint8_t i = 0; i < LAYER_COUNT; i++) {
		for(uint8_t j = 0; j < STAR_COUNT; j++) {
			Star *star = &bg->layers[i].stars[j];

			float scroll_mod = bg->layers[i].scroll_mod;

			Vector2 draw_pos = (Vector2) {
				star->position.x + (bg->offset.x * scroll_mod),
				star->position.y + (bg->offset.y * scroll_mod)
			};
			
			Vector2 wrap_pos = (Vector2) {
				fmod(draw_pos.x, bg->src_rec.width),
				fmod(draw_pos.y, bg->src_rec.height) 
			};

			if(wrap_pos.x < 0) wrap_pos.x += (bg->src_rec.width);
			if(wrap_pos.y < 0) wrap_pos.y += (bg->src_rec.height);

			//DrawCircleV(wrap_pos, 1, colors[i]);
			//DrawCircleV(wrap_pos, star->scale, RAYWHITE);

			uint16_t frame = (bg->sl->anims[star->anim_id].cur_frame + star->frame_offset) % bg->sl->anims[star->anim_id].frame_count;
			DrawSpriteRecolor(
				&bg->sl->spr_pool[star->sprite_id],
				frame,
				wrap_pos,
				0,
				star->scale * 0.1f,
				0,
				colors[star->color_id]
			);
		}
	};

	//EndTextureMode();
}


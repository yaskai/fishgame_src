#include <stdint.h>
#include "raylib.h"
#include "sprites.h"

#ifndef BG_H_
#define BG_H_

#define STAR_COUNT 	 80
#define LAYER_COUNT	  3

typedef struct {
	uint8_t sprite_id;
	uint8_t anim_id;
	uint8_t frame_offset;

	uint8_t color_id;

	float scale;
	Vector2 position;
} Star;

typedef struct {
	float scroll_mod; 

	Star *stars;
} BackgroundLayer;

typedef struct {
	Vector2 offset;

	Rectangle src_rec;
		
	BackgroundLayer layers[LAYER_COUNT];

	RenderTexture2D render_texture;
	
	SpriteLoader *sl;
} Background;

void BgInit(Background *bg, SpriteLoader *sl);
void BgUpdate(Background *bg, float dt);
void BgDraw(Background *bg);

#endif

#include <stdint.h>
#include "raylib.h"

#ifndef BG_H_
#define BG_H_

#define STAR_COUNT 	128
#define LAYER_COUNT	  3

typedef struct {
	Vector2 position;
} Star;

typedef struct {
	float scroll_mod; 

	Star stars[STAR_COUNT];
} BackgroundLayer;

typedef struct {
	Vector2 offset;
		
	BackgroundLayer layers[LAYER_COUNT];

	RenderTexture2D render_texture;
} Background;

void BgInit(Background *bg);
void BgUpdate(Background *bg, float dt);
void BgDraw(Background *bg);

#endif

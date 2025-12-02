#include <string.h>
#include "raylib.h"
#include "bg.h"

void BgInit(Background *bg) {
	bg->render_texture = LoadRenderTexture(1920, 1080);
	
	Star stars[STAR_COUNT] = {0};
	for(uint8_t i = 0; i < STAR_COUNT; i++) {
		stars[i].position = (Vector2){ GetRandomValue(0, 1920), GetRandomValue(0, 1080)};
	}

	memcpy(bg->layers[0].stars, stars, sizeof(stars));
}

void BgUpdate(Background *bg, float dt) {
}

void BgDraw(Background *bg) {
	BeginTextureMode(bg->render_texture);
	ClearBackground((Color){0});

	for(uint8_t i = 0; i < STAR_COUNT; i++) {
		DrawCircleV(bg->layers[0].stars[i].position, 1, RAYWHITE);
	};

	EndTextureMode();
}


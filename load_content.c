#include <stdio.h>
#include "raylib.h"
#include "sprites.h"

//typedef void(*SpriteLoadFunc)(Game *game);
//typedef void(*AudioLoadFunc)(Game *game);

/*
void LoadSpritesAll(SpriteLoader *sl) {
	puts("loading sprites...");

	//LoadSpritesheet("resources/graphics/player_sheet.png", (Vector2){64, 64}, sl);
	//AddSpriteAnim(&sl->spr_pool[0], FrameIndex(&sl->spr_pool[0], 0, 1), 4, 1, sl);

	LoadSpritesheet("resources/graphics/player/SailorSwimmingIdle2.png", (Vector2){144, 144}, sl);
	AddSpriteAnim(&sl->spr_pool[0], 0, sl->spr_pool[0].frame_count, 0.166f, sl);

	//LoadSpritesheet("resources/graphics/asteroid00.png", (Vector2){128, 128}, sl);
	LoadSpritesheet("resources/graphics/asteroid01.png", (Vector2){256, 256}, sl);

	//LoadSpritesheet("resources/graphics/fish01.png", (Vector2){128, 128}, sl);
	LoadSpritesheet("resources/graphics/fish/feeshSwim2.png", (Vector2){144, 81}, sl);
	AddSpriteAnim(&sl->spr_pool[2], 0, sl->spr_pool[2].frame_count, 0.166f, sl);

	LoadSpritesheet("resources/graphics/player/harpoon1.png", (Vector2){51, 38}, sl);
}
*/

void LoadSpritesAll(SpriteLoader *sl) {
	puts("Loading sprites...");

	LoadSpritesheet("resources/graphics/player/SailorSwimmingIdle2.png", (Vector2){144, 144}, sl, SHEET_PLAYER);
	AddSpriteAnim(&sl->spr_pool[SHEET_PLAYER], 0, sl->spr_pool[SHEET_PLAYER].frame_count, 0.166f, sl);

	//LoadSpritesheet("resources/graphics/asteroid00.png", (Vector2){128, 128}, sl);
	LoadSpritesheet("resources/graphics/asteroid01.png", (Vector2){256, 256}, sl, SHEET_ASTEROIDS);

	//LoadSpritesheet("resources/graphics/fish01.png", (Vector2){128, 128}, sl);
	LoadSpritesheet("resources/graphics/fish/feeshSwim2.png", (Vector2){144, 81}, sl, SHEET_FISH);
	AddSpriteAnim(&sl->spr_pool[SHEET_FISH], 0, sl->spr_pool[SHEET_FISH].frame_count, 0.166f, sl);

	LoadSpritesheet("resources/graphics/player/harpoon1.png", (Vector2){51, 38}, sl, SHEET_HARPOON);
}


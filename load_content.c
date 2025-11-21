#include <stdio.h>
#include "raylib.h"
#include "sprites.h"

//typedef void(*SpriteLoadFunc)(Game *game);
//typedef void(*AudioLoadFunc)(Game *game);

void LoadSpritesAll(SpriteLoader *sl) {
	puts("loading sprites...");

	//LoadSpritesheet("resources/graphics/player_sheet.png", (Vector2){64, 64}, sl);
	//AddSpriteAnim(&sl->spr_pool[0], FrameIndex(&sl->spr_pool[0], 0, 1), 4, 1, sl);

	LoadSpritesheet("resources/graphics/player/SailorSwimmingIdle2.png", (Vector2){144, 144}, sl);
	AddSpriteAnim(&sl->spr_pool[0], 0, sl->spr_pool[0].frame_count, 0.166f, sl);

	LoadSpritesheet("resources/graphics/asteroid00.png", (Vector2){128, 128}, sl);

	//LoadSpritesheet("resources/graphics/fish01.png", (Vector2){128, 128}, sl);
	LoadSpritesheet("resources/graphics/fish/feeshSwim2.png", (Vector2){144, 81}, sl);
	AddSpriteAnim(&sl->spr_pool[2], 0, sl->spr_pool[2].frame_count, 0.166f, sl);

	LoadSpritesheet("resources/graphics/player/harpoon1.png", (Vector2){51, 38}, sl);
}


#include <stdio.h>
#include "raylib.h"
#include "sprites.h"

//typedef void(*SpriteLoadFunc)(Game *game);
//typedef void(*AudioLoadFunc)(Game *game);

void LoadSpritesAll(SpriteLoader *sl) {
	puts("Loading sprites...");

	LoadSpritesheet("resources/graphics/player/swim_idle.png", (Vector2){144, 144}, sl, SHEET_PLAYER);
	AddSpriteAnim(&sl->spr_pool[SHEET_PLAYER], 0, sl->spr_pool[SHEET_PLAYER].frame_count, 0.166f, sl, ANIM_PLAYER_SWIM_IDLE);

	LoadSpritesheet("resources/graphics/asteroid01.png", (Vector2){256, 256}, sl, SHEET_ASTEROIDS);

	LoadSpritesheet("resources/graphics/fish/feeshSwim2.png", (Vector2){144, 81}, sl, SHEET_FISH_00);
	AddSpriteAnim(&sl->spr_pool[SHEET_FISH_00], 0, sl->spr_pool[SHEET_FISH_00].frame_count, 0.166f, sl, ANIM_FISH_SWIM_00);

	LoadSpritesheet("resources/graphics/player/harpoon.png", (Vector2){51, 38}, sl, SHEET_HARPOON);
	
	LoadSpritesheet("resources/graphics/player/recoil_lft.png", (Vector2){144, 144}, sl, SHEET_PLAYER_RECOIL_LFT);
	LoadSpritesheet("resources/graphics/player/recoil_rgt.png", (Vector2){144, 144}, sl, SHEET_PLAYER_RECOIL_RGT);

	AddSpriteAnim(
		&sl->spr_pool[SHEET_PLAYER_RECOIL_LFT], 0, sl->spr_pool[SHEET_PLAYER_RECOIL_LFT].frame_count, 0.233f, sl, ANIM_PLAYER_RECOIL_LFT);

	AddSpriteAnim(
		&sl->spr_pool[SHEET_PLAYER_RECOIL_RGT], 0, sl->spr_pool[SHEET_PLAYER_RECOIL_RGT].frame_count, 0.233f, sl, SHEET_PLAYER_RECOIL_RGT);
}


#include <stdio.h>
#include "raylib.h"
#include "sprites.h"

#define PLAYER_SIZE (Vector2){144, 144}
#define STAR_SIZE 	(Vector2){67, 67}

void LoadSpritesAll(SpriteLoader *sl) {
	puts("Loading sprites...");

	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * PLAYER IDLE *

	LoadSpritesheet("resources/graphics/player/swim_idle.png", PLAYER_SIZE, sl, SHEET_PLAYER);
	AddSpriteAnim(&sl->spr_pool[SHEET_PLAYER], 0, sl->spr_pool[SHEET_PLAYER].frame_count, 0.166f, sl, ANIM_PLAYER_SWIM_IDLE);

	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * PLAYER SWIM UP *

	LoadSpritesheet("resources/graphics/player/swim_up.png", PLAYER_SIZE, sl, SHEET_PLAYER_SWIM_UP);
	AddSpriteAnim(&sl->spr_pool[SHEET_PLAYER_SWIM_UP], 0, sl->spr_pool[SHEET_PLAYER_SWIM_UP].frame_count, 0.333f, sl, ANIM_PLAYER_SWIM_UP);
		
	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * ASTEROIDS *

	LoadSpritesheet("resources/graphics/asteroid01.png", (Vector2){256, 256}, sl, SHEET_ASTEROIDS);

	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * HARPOON *
	
	LoadSpritesheet("resources/graphics/player/harpoon.png", (Vector2){51, 38}, sl, SHEET_HARPOON);
	
	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * PLAYER RECOIL *

	LoadSpritesheet("resources/graphics/player/recoil_lft.png", PLAYER_SIZE, sl, SHEET_PLAYER_RECOIL_LFT);
	LoadSpritesheet("resources/graphics/player/recoil_rgt.png", PLAYER_SIZE, sl, SHEET_PLAYER_RECOIL_RGT);

	AddSpriteAnim(
		&sl->spr_pool[SHEET_PLAYER_RECOIL_LFT], 0, sl->spr_pool[SHEET_PLAYER_RECOIL_LFT].frame_count, 0.233f, sl, ANIM_PLAYER_RECOIL_LFT);

	AddSpriteAnim(
		&sl->spr_pool[SHEET_PLAYER_RECOIL_RGT], 0, sl->spr_pool[SHEET_PLAYER_RECOIL_RGT].frame_count, 0.233f, sl, SHEET_PLAYER_RECOIL_RGT);
	
	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * FISH 00 *

	LoadSpritesheet("resources/graphics/fish/fish_00.png", (Vector2){144, 81}, sl, SHEET_FISH_00);
	AddSpriteAnim(&sl->spr_pool[SHEET_FISH_00], 0, sl->spr_pool[SHEET_FISH_00].frame_count, 0.166f, sl, ANIM_FISH_SWIM_00);

	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * FISH 01 *

	LoadSpritesheet("resources/graphics/fish/fish_01.png", (Vector2){144, 60}, sl, SHEET_FISH_01);
	AddSpriteAnim(&sl->spr_pool[SHEET_FISH_01], 0, sl->spr_pool[SHEET_FISH_01].frame_count - 2, 0.166f, sl, ANIM_FISH_SWIM_01);
	
	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * FISH 02 *

	LoadSpritesheet("resources/graphics/fish/fish_02.png", (Vector2){144, 62}, sl, SHEET_FISH_02);
	AddSpriteAnim(&sl->spr_pool[SHEET_FISH_02], 0, sl->spr_pool[SHEET_FISH_02].frame_count - 2, 0.166f, sl, ANIM_FISH_SWIM_02);

	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * FISH 03 *

	LoadSpritesheet("resources/graphics/fish/fish_03.png", (Vector2){144, 74}, sl, SHEET_FISH_03);
	AddSpriteAnim(&sl->spr_pool[SHEET_FISH_03], 0, sl->spr_pool[SHEET_FISH_03].frame_count, 0.166f, sl, ANIM_FISH_SWIM_03);

	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * BACKGROUND STARS *

	LoadSpritesheet("resources/graphics/background/medium.png",			STAR_SIZE, sl, SHEET_STAR_00);
	LoadSpritesheet("resources/graphics/background/slow.png",			STAR_SIZE, sl, SHEET_STAR_01);
	LoadSpritesheet("resources/graphics/background/slow_short.png", 	STAR_SIZE, sl, SHEET_STAR_02);
	LoadSpritesheet("resources/graphics/background/medium_short.png", 	STAR_SIZE, sl, SHEET_STAR_03);

	AddSpriteAnim(&sl->spr_pool[SHEET_STAR_00], 0, sl->spr_pool[SHEET_STAR_00].frame_count, 0.466f, sl, ANIM_STAR_00);
	AddSpriteAnim(&sl->spr_pool[SHEET_STAR_01], 0, sl->spr_pool[SHEET_STAR_01].frame_count, 0.466f, sl, ANIM_STAR_01);
	AddSpriteAnim(&sl->spr_pool[SHEET_STAR_02], 0, sl->spr_pool[SHEET_STAR_02].frame_count, 0.466f, sl, ANIM_STAR_02);
	AddSpriteAnim(&sl->spr_pool[SHEET_STAR_03], 0, sl->spr_pool[SHEET_STAR_03].frame_count, 0.466f, sl, ANIM_STAR_03);
}

